/*------------------------------------------------------------
 *  datasetcreator.cpp
 *  Created:  10. July 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "datasetcreator.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QErrorMessage>
#include <QDirIterator>
#include <QThreadPool>

#include <chrono>
using namespace std::chrono;


DatasetCreator::DatasetCreator(DatasetConfig *datasetConfig) : m_datasetConfig(datasetConfig) {
	qRegisterMetaType<QList<cv::Mat>>();

}

void DatasetCreator::createDatasetSlot(QList<QString> recordings, QList<QString> entities, QList<QString> keypoints) {
	m_recordingsList = recordings;
	m_entitiesList = entities;
	m_keypointsList = keypoints;
	for (const auto & recording : m_recordingsList) {
		QString path = recording.split('/').takeLast();
		QList<QString> cameras = getCameraNames(recording);
		if (m_datasetConfig->dataType == "Videos") {
			m_datasetConfig->videoFormat = getVideoFormat(recording);
			if (m_datasetConfig->videoFormat == "") {
				emit datasetCreationFailed("All videos must have the same format!");
				return;
			}
			if (!checkFrameCounts(recording, cameras)) {
				emit datasetCreationFailed("Frame count mismatch!");
				return;
			}
		}
		QList<int> frameNumbers = extractFrames(recording, cameras);
		createSavefile(recording, cameras, m_datasetConfig->datasetPath + "/" + m_datasetConfig->datasetName + '/' + path, frameNumbers);
	}
	emit datasetCreated();
}

QList<QString> DatasetCreator::getCameraNames(const QString& path) {
	QList<QString> cameraNames;
	if (m_datasetConfig->dataType == "Images") {
		for (QDirIterator it(path); it.hasNext();) {
			QString subpath = it.next();
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				cameraNames.append(suffix);
			}
		}
	}
	else if (m_datasetConfig->dataType == "Videos") {
		for (QDirIterator it(path); it.hasNext();) {
			QString subpath = it.next();
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				suffix = suffix.split(".").takeFirst();
				cameraNames.append(suffix);
			}
		}
	}
	return cameraNames;
}

QString DatasetCreator::getVideoFormat(const QString& recording) {
	QList <QString> formats;
	for (QDirIterator it(recording); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (suffix != "." && suffix != "..") {
			suffix = suffix.split(".").takeLast();
			formats.append(suffix);
		}
	}
	if (formats.count(formats[0]) != formats.length()) {
		return "";
	}
	else {
		return formats[0];
	}
}


bool DatasetCreator::checkFrameCounts(const QString& recording, QList<QString> cameras) {
	int numFrames = -1;
	for (const auto & camera : cameras) {
		cv::VideoCapture cap((recording + "/" + camera + "." + m_datasetConfig->videoFormat).toStdString());
		if(!cap.isOpened()){
    	std::cout << "Error opening video stream or file" << std::endl;
    	return false;
  	}

		int newNumFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
		cap.release();
		if (numFrames != -1 && newNumFrames != numFrames)  {
			return false;
		}
		else {
			numFrames = newNumFrames;
		}
	}
	return true;
}

QList<int> DatasetCreator::extractFrames(const QString& recording, QList<QString> cameras) {
	QList<int> frameNumbers;
	cv::VideoCapture cap((recording + "/" + cameras[0] + "." + m_datasetConfig->videoFormat).toStdString());
	float numFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
	std::cout << "Total Num Frames: " << static_cast<int>(numFrames) << std::endl;
	cap.release();
	if (m_datasetConfig->dataType == "Videos") {
		if (m_datasetConfig->samplingMethod == "uniform") {
			float spacing = numFrames / (m_datasetConfig->frameSetsRecording+0.5);
			for (float num = 0.; num < numFrames; num += spacing) {
				frameNumbers.append(static_cast<int>(num+spacing/2.));
			}
		}
		else if (m_datasetConfig->samplingMethod == "kmeans") {
			int subsamplingRate = 10;

			QList <VideoStreamer*> streamers;
			QThreadPool *threadPool = QThreadPool::globalInstance();
			for (int i = 0; i < m_datasetConfig->numCameras; i++) {
				VideoStreamer *streamer = new VideoStreamer(recording + "/" + cameras[i] + "." + m_datasetConfig->videoFormat, i);
				connect(streamer, &VideoStreamer::computedDCTs, this, &DatasetCreator::computedDCTsSlot);
				streamers.append(streamer);
	 			threadPool->start(streamer);
			}
			threadPool->waitForDone();

			QEventLoop loop;
			connect(this, &DatasetCreator::gotAllDCTs, &loop, &QEventLoop::quit);
			loop.exec();

			cv::Mat dctImagesList;
			for (int i = 0; i < m_dctMap[0].size(); i++) {
				cv::Mat dctImages;
				for (const auto dctImage : m_dctMap) {
					if (dctImages.empty()) {
						dctImages = dctImage[i];
					}
					else {
						cv::vconcat(dctImages, dctImage[i], dctImages);
					}
				}
				if (dctImagesList.empty()) {
					dctImagesList = dctImages.reshape(1,1);
				}
				else {
					cv::vconcat(dctImagesList,dctImages.reshape(1,1),dctImagesList);
				}
			}

			cv::Mat labels, centers;
			std::cout << dctImagesList.size() << std::endl;
			double compactness = cv::kmeans(dctImagesList, m_datasetConfig->frameSetsRecording, labels,
					cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 1000, 1e-4),
						 25, cv::KMEANS_PP_CENTERS, centers);

			for (int i = 0; i < m_datasetConfig->frameSetsRecording; i++) {
				std::vector< float > sums;
				cv::Mat clusterDists;
				for (int j = 0; j < dctImagesList.size().height; j++) {
					clusterDists = dctImagesList.row(j)-centers.row(i);
					sums.push_back(cv::sum(clusterDists.mul(clusterDists))[0]);
				}
				int minElementIndex = std::min_element(sums.begin(),sums.end()) - sums.begin();
				frameNumbers.append(minElementIndex*subsamplingRate);
			}
		}
	}
	return frameNumbers;
}



QList<QString> DatasetCreator::getAndCopyFrames(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers) {
	QList<QString> frameNames;
	if (m_datasetConfig->dataType == "Videos") {
		QList <ImageWriter*> writers;
		QThreadPool *threadPool = QThreadPool::globalInstance();
		for (const auto & camera : cameras) {
			QString videoPath = recording + "/" + camera + "." + m_datasetConfig->videoFormat;
			QString destinationPath = dataFolder + "/" + camera;
			ImageWriter *writer = new ImageWriter(videoPath, destinationPath, frameNumbers);
			writers.append(writer);
			threadPool->start(writer);
		}
		for (const auto &frameNumber : frameNumbers) {
			frameNames.append("Frame_" + QString::number(frameNumber) + ".jpg");
		}
		threadPool->waitForDone();
	}
	return frameNames;
}

void DatasetCreator::createSavefile(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers) {
	QList<QFile*> saveFiles;
	for (const auto & camera : cameras) {
		QDir dir;
		dir.mkpath(dataFolder + "/" + camera);
		QFile *file = new QFile(dataFolder + "/" + camera +"/annotations.csv");
		saveFiles.append(file);
		if (!file->open(QIODevice::WriteOnly)) {
			std::cout << "Can't open File" << std::endl;
			QErrorMessage *msg = new QErrorMessage();
			msg->showMessage("Error writing savefile. Make sure you have the right permissions...");
			return;
		}
		 QTextStream stream(file);
		 stream << "Scorer";
		 int num_columns = m_keypointsList.size()*3*m_entitiesList.size();
		 for (int i = 0; i < num_columns; i++) {
			 stream << "," << "Scorer";
		 }
		 stream << "\n";
		 stream << "entities";
		 for (int i = 0; i < num_columns; i++) {
			 stream << "," << m_entitiesList[i/(m_keypointsList.size()*3)];
		 }
		 stream << "\n";
		 stream << "bodyparts";
		 for (int i = 0; i < num_columns; i++) {
			 stream << "," << m_keypointsList[i/(m_entitiesList.size()*3)];
		 }
		 stream << "\n";
		 stream << "coords";
		 for (int i = 0; i < num_columns; i++) {
			 if (i%3 == 1) stream << "," << "x";
			 else if (i%3 == 0) stream << "," << "y";
			 else stream << "," << "state";

		 }
		 stream << "\n";
	}
	QList<QString> frameNames = getAndCopyFrames(recording, cameras, dataFolder, frameNumbers);

	for (int cam = 0; cam < m_datasetConfig->numCameras; cam++) {
		for (const auto &frameName : frameNames) {
			QTextStream stream(saveFiles[cam]);
			stream << frameName << ",";
			for (int i = 0; i < m_keypointsList.size()*m_entitiesList.size(); i++) {
					stream << ",," << 0 << ",";
			}
		 stream << "\n";
		}
		saveFiles[cam]->close();
	}
}

void DatasetCreator::computedDCTsSlot(QList<cv::Mat> dctImages, int threadNumber) {
	m_dctMap[threadNumber] = dctImages;
	if (m_dctMap.size() == 12) {
		emit gotAllDCTs();
	}
}


VideoStreamer::VideoStreamer(const QString &videoPath, int threadNumber) {
	m_cap = new cv::VideoCapture(videoPath.toStdString());
	m_threadNumber = threadNumber;
}

void VideoStreamer::run() {
	bool readFrame = true;
	int frameCount = 0;
	while (readFrame == true) {
		if (m_threadNumber == 0 && frameCount % 100 == 0) {
			std::cout << frameCount << std::endl;
		}
		cv::Mat img,dctImage;
		m_cap->set(cv::CAP_PROP_POS_FRAMES, frameCount*10);
		readFrame = m_cap->read(img);
		if (readFrame) {
			frameCount++;
			cv::resize(img, img, cv::Size(160, 128));
			cv::cvtColor(img,img, cv::COLOR_BGR2GRAY);
			img.convertTo(img, CV_32FC1);
			cv::dct(img/255.0, dctImage);
			dctImage = dctImage(cv::Rect(0,0,20,16));
			dctImages.append(dctImage);
		}
	}
	emit computedDCTs(dctImages, m_threadNumber);
}


ImageWriter::ImageWriter(const QString &videoPath, const QString &destinationPath, QList<int> frameNumbers) :
	m_destinationPath(destinationPath), m_frameNumbers(frameNumbers) {
		m_cap = new cv::VideoCapture(videoPath.toStdString());
}

void ImageWriter::run() {
	for (const auto & frameNumber : m_frameNumbers) {
		cv::Mat frame;
		m_cap->set(cv::CAP_PROP_POS_FRAMES, frameNumber-1);
		if (m_cap->read(frame)) {
			cv::imwrite((m_destinationPath + "/" +  "Frame_" + QString::number(frameNumber) + ".jpg").toStdString(), frame);
		}
		else {
			std::cout << "Error reading Frame"<< std::endl;
			return;
		}
	}
}
