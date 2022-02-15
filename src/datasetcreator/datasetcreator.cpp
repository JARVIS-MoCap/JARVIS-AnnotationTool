/*******************************************************************************
 * File:			  datasetcreator.cpp
 * Created: 	  10. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/

#include "datasetcreator.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDirIterator>
#include <QThreadPool>

#include <fstream>
#include <chrono>
using namespace std::chrono;


DatasetCreator::DatasetCreator(DatasetConfig *datasetConfig) :
			m_datasetConfig(datasetConfig) {
	qRegisterMetaType<QList<cv::Mat>>();

}


void DatasetCreator::createDatasetSlot(QList<RecordingItem> recordings,
			QList<QString> entities, QList<QString> keypoints,
			QList<SkeletonComponent> skeleton) {
	delayl(100); 	//Give the gui Thread time to pop up the progressWindow
	m_creationCanceled = false;
	m_recordingItems = recordings;
	m_entitiesList = entities;
	m_keypointsList = keypoints;
	m_skeleton = skeleton;
	for (const auto & recording : m_recordingItems) {
		m_dctMap.clear();
		QList<QString> cameras = getCameraNames(recording.path);	//TODO: Check if all recordings in one Dataset have the same cameras!
		m_datasetConfig->numCameras = cameras.size();
		emit recordingBeingProcessedChanged(recording.name, cameras);
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
			cv::VideoCapture cap((recording.path + "/" + cameras[0] + "." +
											      m_datasetConfig->videoFormat).toStdString());
			fullWindow.name = recording.name;
			fullWindow.start = 0;
	    fullWindow.end = cap.get(cv::CAP_PROP_FRAME_COUNT);
			cap.release();
			timeLineWindows.append(fullWindow);
			emit currentSegmentChanged("");
			QList<int> frameNumbers = extractFrames(recording.path, timeLineWindows,
						cameras);
			if (!m_creationCanceled) {
				QString savepath;
					savepath = m_datasetConfig->datasetPath + "/" +
									   m_datasetConfig->datasetName + "/" + recording.name;
				createSavefile(recording.path, cameras, savepath, frameNumbers);
			}
		}
		QMap<QString, QList<TimeLineWindow>> recordingSubsets =
					getRecordingSubsets(recording.timeLineList);

		for (const auto &subsetName : recordingSubsets.keys()) {
			emit currentSegmentChanged(subsetName);
			QList<int> frameNumbers = extractFrames(recording.path,
						recordingSubsets[subsetName], cameras);
			if (!m_creationCanceled) {
				QString savepath;
				savepath = m_datasetConfig->datasetPath + "/" +
									 m_datasetConfig->datasetName + "/" +
									 recording.name + "/" + subsetName;
				createSavefile(recording.path, cameras, savepath, frameNumbers);
			}
			else {
				break;
			}
		}
	}

	if (!m_creationCanceled) {
		emit datasetCreated();
	}
	createDatasetConfigFile(m_datasetConfig->datasetPath);
}


void DatasetCreator::createDatasetConfigFile(const QString& path) {
	YAML::Node config;  // starts out as null

	config["Name"] = m_datasetConfig->datasetName.toStdString();
	config["Date of creation"] =
				QDate::currentDate().toString(Qt::ISODate).toStdString();
	for (const auto & recording : m_recordingItems) {
		config["Recordings"][recording.name.toStdString()] = YAML::Node();
		for (const auto &segment : recording.timeLineList) {
			config["Recordings"][recording.name.toStdString()].push_back(
						segment.name.toStdString());
		}
	}

	for (const auto& camera : getCameraNames(m_recordingItems[0].path)) {
		config["Cameras"].push_back(camera.toStdString());
	}

	for (const auto & entity : m_entitiesList) {
		config["Entities"].push_back(entity.toStdString());
	}

	for (const auto & keypoint : m_keypointsList) {
		config["Keypoints"].push_back(keypoint.toStdString());
	}

	for (const auto & bone : m_skeleton) {
		//config["Skeleton"].push_back(bone[0].toStdString());
		config["Skeleton"][bone.name.toStdString()]["Keypoints"].push_back(
					bone.keypointA.toStdString());
		config["Skeleton"][bone.name.toStdString()]["Keypoints"].push_back(
					bone.keypointB.toStdString());
		config["Skeleton"][bone.name.toStdString()]["Length"].push_back(
					bone.length);
	}

	std::ofstream configStream((path + "/" + m_datasetConfig->datasetName + "/" +
				m_datasetConfig->datasetName +  ".yaml").toStdString());
	configStream << config;
}


QList<QString> DatasetCreator::getCameraNames(const QString& path) {
	QList<QString> cameraNames;
	for (QDirIterator it(path); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (suffix != "." && suffix != "..") {
			if (m_datasetConfig->validRecordingFormats.contains(
						suffix.split(".").takeLast())) {
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


bool DatasetCreator::checkFrameCounts(const QString& recording,
			QList<QString> cameras) {
	int numFrames = -1;
	for (const auto & camera : cameras) {
		cv::VideoCapture cap((recording + "/" + camera + "." +
					m_datasetConfig->videoFormat).toStdString());
		if(!cap.isOpened()){
			emit datasetCreationFailed("Error opening video stream or file");
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


QMap<QString, QList<TimeLineWindow>> DatasetCreator::getRecordingSubsets(
			QList<TimeLineWindow> timeLineWindows) {
	QMap<QString, QList<TimeLineWindow>> recordingSubsets;

	for (const auto &window : timeLineWindows) {
		recordingSubsets[window.name].append(window);
	}
	return recordingSubsets;
}


QList<int> DatasetCreator::extractFrames(const QString &path,
			QList<TimeLineWindow> timeLineWindows, QList<QString> cameras) {
	QList<int> frameNumbers;

	if (m_datasetConfig->samplingMethod == "uniform") {
		float totalNumFrames = 0;
		for (const auto& window : timeLineWindows) {
			totalNumFrames += window.end-window.start;
		}
		float spacing = totalNumFrames / (m_datasetConfig->frameSetsRecording+1);
		int  remainder = 0;
		for (const auto& window : timeLineWindows) {
			for (float num = window.start+spacing-remainder; num < window.end;
						num += spacing) {
				frameNumbers.append(static_cast<int>(num));
				remainder = window.end - num;
			}
		}
	}

	else if (m_datasetConfig->samplingMethod == "kmeans") {
		QList <VideoStreamer*> streamers;
		QThreadPool *threadPool = QThreadPool::globalInstance();
		for (int i = 0; i < m_datasetConfig->numCameras; i++) {
			VideoStreamer *streamer = new VideoStreamer(path + "/" + cameras[i] +
						"." + m_datasetConfig->videoFormat, timeLineWindows,
						m_datasetConfig->frameSetsRecording, i);
			connect(streamer, &VideoStreamer::computedDCTs,
							this, &DatasetCreator::computedDCTsSlot);
			connect(this, &DatasetCreator::creationCanceled,
							streamer, &VideoStreamer::creationCanceledSlot);
			connect(streamer, &VideoStreamer::dctProgress,
							this, &DatasetCreator::dctProgress);
			streamers.append(streamer);
 			threadPool->start(streamer);
		}
		while (!threadPool->waitForDone(10)) {
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
					dctImages = dctImage[i].clone();
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
		cv::kmeans(dctImagesList, m_datasetConfig->frameSetsRecording, labels,
					cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT,
					1000, 1e-4), 25, cv::KMEANS_PP_CENTERS, centers);

		for (int i = 0; i < m_datasetConfig->frameSetsRecording; i++) {
			std::vector< float > sums;
			cv::Mat clusterDists;
			for (int j = 0; j < dctImagesList.size().height; j++) {
				clusterDists = dctImagesList.row(j)-centers.row(i);
				sums.push_back(cv::sum(clusterDists.mul(clusterDists))[0]);
			}
			int minElementIndex = std::min_element(sums.begin(),
																						 sums.end()) - sums.begin();
			frameNumbers.append(m_frameNumberMap[minElementIndex]);
		}
		emit finishedClustering();
	}

	return frameNumbers;
}


QList<QString> DatasetCreator::getAndCopyFrames(const QString& recording,
				QList<QString> cameras, const QString& dataFolder,
				QList<int> frameNumbers) {
	QList<QString> frameNames;
	QList <ImageWriter*> writers;
	QThreadPool *threadPool = QThreadPool::globalInstance();
	int threadNumber = 0;
	for (const auto & camera : cameras) {
		QString videoPath = recording + "/" + camera + "." +
					m_datasetConfig->videoFormat;
		QString destinationPath = dataFolder + "/" + camera;
		ImageWriter *writer = new ImageWriter(videoPath, destinationPath,
					frameNumbers, threadNumber);
		connect(this, &DatasetCreator::creationCanceled,
						writer, &ImageWriter::creationCanceledSlot);
		connect(writer, &ImageWriter::copyImagesStatus,
						this, &DatasetCreator::copyImagesStatus);
		threadNumber++;
		writers.append(writer);
		threadPool->start(writer);
	}
	for (const auto &frameNumber : frameNumbers) {
		frameNames.append("Frame_" + QString::number(frameNumber) + ".jpg");
	}
	while (!threadPool->waitForDone(10)) {
		QCoreApplication::instance()->processEvents();
	}
	return frameNames;
}


void DatasetCreator::createSavefile(const QString& recording,
			QList<QString> cameras, const QString& dataFolder,
			QList<int> frameNumbers) {
	for (const auto & camera : cameras) {
		QDir dir;
		dir.mkpath(dataFolder + "/" + camera);
	}
	QList<QString> frameNames = getAndCopyFrames(recording, cameras, dataFolder,
				frameNumbers);

	for (const auto & camera : cameras) {
		QFile file = QFile(dataFolder + "/" + camera + "/annotations.csv");
		if (!file.open(QIODevice::WriteOnly)) {
			emit datasetCreationFailed("Can't open file " + dataFolder + "/" +
						camera + "/annotations.csv" + " !");;
			return;
		}
		 QTextStream stream(&file);
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
			 stream << "," << m_keypointsList[(i/3)%m_keypointsList.size()];
		 }
		 stream << "\n";
		 stream << "coords";
		 for (int i = 0; i < num_columns; i++) {
			 if (i%3 == 1) stream << "," << "x";
			 else if (i%3 == 0) stream << "," << "y";
			 else stream << "," << "state";
		 }
		 stream << "\n";

		 for (const auto &frameName : frameNames) {
			 stream << frameName << ",";
			 for (int i = 0; i < m_keypointsList.size()*m_entitiesList.size(); i++) {
					 stream << ",," << 0 << ",";
			 }
			stream << "\n";
		 }
		 file.close();
	}
}


void DatasetCreator::computedDCTsSlot(QList<cv::Mat> dctImages,
			QMap<int,int> frameNumberMap, int threadNumber) {
	m_dctMap[threadNumber] = dctImages;
	m_frameNumberMap = frameNumberMap;
	if (m_dctMap.size() == m_datasetConfig->numCameras) {
		emit gotAllDCTs();
	}
}


void DatasetCreator::cancelCreationSlot() {
	m_creationCanceled = true;
	emit creationCanceled();
}
