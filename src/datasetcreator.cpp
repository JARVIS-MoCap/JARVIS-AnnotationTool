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

void DatasetCreator::createDatasetSlot(QList<RecordingItem> recordings, QList<QString> entities, QList<QString> keypoints) {
	m_recordingItems = recordings;
	m_entitiesList = entities;
	m_keypointsList = keypoints;
	for (const auto & recording : m_recordingItems) {
		emit recordingBeingProcessedChanged(recording.name);
		QString path = recording.path.split('/').takeLast();
		QList<QString> cameras = getCameraNames(recording.path);
		m_datasetConfig->videoFormat = getVideoFormat(recording.path);
		if (m_datasetConfig->videoFormat == "") {
			emit datasetCreationFailed("All videos must have the same format!");
			return;
		}
		if (!checkFrameCounts(recording.path, cameras)) {
			emit datasetCreationFailed("Frame count mismatch!");
			return;
		}
		if (recording.timeLineList.size() == 0) {
			QList<TimeLineWindow> timeLineWindows;
			TimeLineWindow fullWindow;
			cv::VideoCapture cap((recording.path + "/" + cameras[0] + "." + m_datasetConfig->videoFormat).toStdString());
			fullWindow.name = recording.name;
			fullWindow.start = 0;
	    fullWindow.end = cap.get(cv::CAP_PROP_FRAME_COUNT);
			cap.release();
			timeLineWindows.append(fullWindow);
			emit currentSegmentChanged("");
			QList<int> frameNumbers = extractFrames(recording.path, timeLineWindows, cameras);
			createSavefile(recording.path, cameras, m_datasetConfig->datasetPath + "/" + m_datasetConfig->datasetName + "/" + recording.name, frameNumbers);
		}
		QMap<QString, QList<TimeLineWindow>> recordingSubsets = getRecordingSubsets(recording.timeLineList);
		for (const auto &subsetName : recordingSubsets.keys()) {
			emit currentSegmentChanged(subsetName);
			QList<int> frameNumbers = extractFrames(recording.path, recordingSubsets[subsetName], cameras);
			createSavefile(recording.path, cameras, m_datasetConfig->datasetPath + "/" + m_datasetConfig->datasetName + "/" + path + "/" + subsetName, frameNumbers);
		}
	}
	emit datasetCreated();
}

QList<QString> DatasetCreator::getCameraNames(const QString& path) {
	QList<QString> cameraNames;
	for (QDirIterator it(path); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (suffix != "." && suffix != "..") {
			if (m_datasetConfig->validRecordingFormats.contains(suffix.split(".").takeLast())) {
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
			if (m_datasetConfig->validRecordingFormats.contains(suffix)) {
				formats.append(suffix);
			}
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

QMap<QString, QList<TimeLineWindow>> DatasetCreator::getRecordingSubsets(QList<TimeLineWindow> timeLineWindows) {
	QMap<QString, QList<TimeLineWindow>> recordingSubsets;
	for (const auto &window : timeLineWindows) {
		recordingSubsets[window.name].append(window);
	}
	std::cout << "SubsetSize: " << recordingSubsets.size() << std::endl;
	return recordingSubsets;
}

QList<int> DatasetCreator::extractFrames(const QString &path, QList<TimeLineWindow> timeLineWindows, QList<QString> cameras) {
	QList<int> frameNumbers;
	if (m_datasetConfig->samplingMethod == "uniform") {
		float totalNumFrames = 0;
		for (const auto& window : timeLineWindows) {
			totalNumFrames += window.end-window.start;
		}
		float spacing = totalNumFrames / (m_datasetConfig->frameSetsRecording+1);
		int  remainder = 0;
		for (const auto& window : timeLineWindows) {
			for (float num = window.start+spacing-remainder; num < window.end; num += spacing) {
				frameNumbers.append(static_cast<int>(num));
				remainder = window.end - num;
			}
		}
	}
	else if (m_datasetConfig->samplingMethod == "kmeans") {
		QList <VideoStreamer*> streamers;
		QThreadPool *threadPool = QThreadPool::globalInstance();
		for (int i = 0; i < m_datasetConfig->numCameras; i++) {
			VideoStreamer *streamer = new VideoStreamer(path + "/" + cameras[i] + "." + m_datasetConfig->videoFormat, timeLineWindows, m_datasetConfig->frameSetsRecording, i);
			connect(streamer, &VideoStreamer::computedDCTs, this, &DatasetCreator::computedDCTsSlot);
			if (i == 0) {
				connect(streamer, &VideoStreamer::dctProgress, this, &DatasetCreator::dctProgress);
			}
			streamers.append(streamer);
 			threadPool->start(streamer);
		}
		while (!threadPool->waitForDone(100)) {
			QCoreApplication::instance()->processEvents();
		}

		QEventLoop loop;
		connect(this, &DatasetCreator::gotAllDCTs, &loop, &QEventLoop::quit);
		loop.exec();

		emit startedClustering();

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
			frameNumbers.append(m_frameNumberMap[minElementIndex]);
		}
		emit finishedClustering();
	}

	return frameNumbers;
}



QList<QString> DatasetCreator::getAndCopyFrames(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers) {
	QList<QString> frameNames;
	QList <ImageWriter*> writers;
	QThreadPool *threadPool = QThreadPool::globalInstance();
	int threadNumber = 0;
	for (const auto & camera : cameras) {
		QString videoPath = recording + "/" + camera + "." + m_datasetConfig->videoFormat;
		QString destinationPath = dataFolder + "/" + camera;
		ImageWriter *writer = new ImageWriter(videoPath, destinationPath, frameNumbers, threadNumber);
		if (threadNumber == 0) {
			connect(writer, &ImageWriter::copyImagesStatus, this, &DatasetCreator::copyImagesStatus);
		}
		threadNumber++;
		writers.append(writer);
		threadPool->start(writer);
	}
	for (const auto &frameNumber : frameNumbers) {
		frameNames.append("Frame_" + QString::number(frameNumber) + ".jpg");
	}
	while (!threadPool->waitForDone(100)) {
		QCoreApplication::instance()->processEvents();
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

void DatasetCreator::computedDCTsSlot(QList<cv::Mat> dctImages, QMap<int,int> frameNumberMap, int threadNumber) {
	m_dctMap[threadNumber] = dctImages;
	m_frameNumberMap = frameNumberMap;
	if (m_dctMap.size() == m_datasetConfig->numCameras) {
		emit gotAllDCTs();
	}
}


VideoStreamer::VideoStreamer(const QString &videoPath, QList<TimeLineWindow> timeLineWindows, int numFramesToExtract, int threadNumber) {
	m_cap = new cv::VideoCapture(videoPath.toStdString());
	m_threadNumber = threadNumber;
	m_timeLineWindows = timeLineWindows;
	m_numFramesToExtract = numFramesToExtract;
}

void VideoStreamer::run() {
	bool readFrame = true;
	int frameCount = 0;
	int indexCount = 0;
	int totalFrames = 0;
	int minFrameCount = 0;
	int subSamplingRate = 10;
	cv::Mat img,dctImage;
	for (const auto & window : m_timeLineWindows) {
		int windowSize = window.end-window.start;
		totalFrames += windowSize;
		if (minFrameCount == 0 || windowSize < minFrameCount) minFrameCount = windowSize;
	}
	while (m_numFramesToExtract > minFrameCount/subSamplingRate) {
		subSamplingRate = subSamplingRate/2;
	}

	for (const auto & window : m_timeLineWindows) {
		frameCount = window.start;
		while (frameCount < window.end) {
			if (m_threadNumber == 0 && indexCount % 1 == 0) {
				emit dctProgress(indexCount, totalFrames/subSamplingRate);
			}
			m_cap->set(cv::CAP_PROP_POS_FRAMES, frameCount);
			readFrame = m_cap->read(img);
			if (readFrame) {
				cv::resize(img, img, cv::Size(160, 128));
				cv::cvtColor(img,img, cv::COLOR_BGR2GRAY);
				img.convertTo(img, CV_32FC1);
				cv::dct(img/255.0, dctImage);
				dctImage = dctImage(cv::Rect(0,0,20,16));
				m_dctImages.append(dctImage);
				m_frameNumberMap[indexCount++] = frameCount;
				frameCount += subSamplingRate;
			}
		}
	}
	emit computedDCTs(m_dctImages, m_frameNumberMap, m_threadNumber);
}


ImageWriter::ImageWriter(const QString &videoPath, const QString &destinationPath, QList<int> frameNumbers, int threadNumber) :
	m_destinationPath(destinationPath), m_frameNumbers(frameNumbers), m_threadNumber(threadNumber) {
		m_cap = new cv::VideoCapture(videoPath.toStdString());
}

void ImageWriter::run() {
	int totalNumFrames = m_frameNumbers.size();
	int frameCount = 0;

	for (const auto & frameNumber : m_frameNumbers) {
		cv::Mat frame;
		m_cap->set(cv::CAP_PROP_POS_FRAMES, frameNumber-1);
		if (m_cap->read(frame)) {
			cv::imwrite((m_destinationPath + "/" +  "Frame_" + QString::number(frameNumber) + ".jpg").toStdString(), frame);
			frameCount++;
		}
		else {
			std::cout << "Error reading Frame"<< std::endl;
			return;
		}
		if (m_threadNumber == 0 && frameCount % 1 == 0) {
			emit copyImagesStatus(frameCount, totalNumFrames);
		}
	}
}
