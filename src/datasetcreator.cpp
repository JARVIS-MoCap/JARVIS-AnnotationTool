/*------------------------------------------------------------
 *  datasetcreator.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "datasetcreator.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QErrorMessage>
#include <QDirIterator>


DatasetCreator::DatasetCreator(DatasetConfig *datasetConfig) : m_datasetConfig(datasetConfig) {
}

void DatasetCreator::createDatasetSlot(QList<QString> recordings, QList<QString> entities, QList<QString> keypoints) {
	std::cout << "Creating Dataset!" << std::endl;
	m_recordingsList = recordings;
	m_entitiesList = entities;
	m_keypointsList = keypoints;
	for (const auto & recording : m_recordingsList) {
		QString path = recording.split('/').takeLast();
		QList<QString> cameras = getCameraNames(recording);
		if (m_datasetConfig->dataType == "Videos") {
			m_datasetConfig->videoFormat = getVideoFormat(recording);
			if (m_datasetConfig->videoFormat == "") {
				std::cout << "All videos must have the same format!" << std::endl;
				return;
			}
			if (!checkFrameCounts(recording, cameras)) {
				std::cout << "Frame count mismatch!!" << std::endl;
				return;
			}
		}
		QList<int> frameNumbers = extractFrames(recording, cameras);
		createSavefile(recording, cameras, m_datasetConfig->datasetName + '/' + path, frameNumbers);
	}

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
	if (m_datasetConfig->dataType == "Videos") {
		if (m_datasetConfig->samplingMethod == "uniform") {
			cv::VideoCapture cap((recording + "/" + cameras[0] + "." + m_datasetConfig->videoFormat).toStdString());
			float numFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
			cap.release();
			float spacing = numFrames / m_datasetConfig->frameSetsRecording;
			for (float num = 0.; num < numFrames; num += spacing) {
				frameNumbers.append(static_cast<int>(num+spacing/2.));
			}
		}
	}
	return frameNumbers;
}

QList<QString> DatasetCreator::getAndCopyFrames(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers) {
	QList<QString> frameNames;
	if (m_datasetConfig->dataType == "Videos") {
		for (const auto & camera : cameras) {
			cv::VideoCapture cap((recording + "/" + camera + "." + m_datasetConfig->videoFormat).toStdString());
			if(!cap.isOpened()) {
				std::cout << "Error opening video stream or file" << std::endl;
				return frameNames;
			}
			for (const auto & frameNumber : frameNumbers) {
				cv::Mat frame;
				cap.set(cv::CAP_PROP_POS_FRAMES, frameNumber);
    		cap >> frame;
				cv::imwrite((dataFolder + "/" + camera + "/" +  "Frame_" + QString::number(frameNumber) + ".jpg").toStdString(), frame);
			}
			cap.release();
		}
		for (const auto &frameNumber : frameNumbers) {
			frameNames.append("Frame_" + QString::number(frameNumber) + ".jpg");
		}
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
			 stream << "," << m_keypointsList[i%m_entitiesList.size()];
		 }
		 stream << "\n";
		 stream << "coords";
		 for (int i = 0; i < num_columns; i++) {
			 if (i%3 == 1) stream << "," << "x";
			 else if (i%3 == 2) stream << "," << "state";
			 else stream << "," << "y";
		 }
		 stream << "\n";
	}
	QList<QString> frameNames = getAndCopyFrames(recording, cameras, dataFolder, frameNumbers);

	for (int cam = 0; cam < m_datasetConfig->numCameras; cam++) {
		for (const auto &frameName : frameNames) {
			QTextStream stream(saveFiles[cam]);
			stream << frameName;
			for (int i = 0; i < m_keypointsList.size()*3*m_entitiesList.size(); i++) {
					stream << ",," << 0 << ",";
			}
		 stream << "\n";
		}
		saveFiles[cam]->close();
	}
}
