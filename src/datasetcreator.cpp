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
		std::cout << cameras.size() << std::endl;

		createSavefile(m_datasetConfig->datasetName + '/' + path, cameras);
	}

}

QList<QString> DatasetCreator::getCameraNames(QString path) {
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
				std::cout << suffix.toStdString() << std::endl;
				cameraNames.append(suffix);
			}
		}
	}
	return cameraNames;
}

void DatasetCreator::createSavefile(const QString& dataFolder, QList<QString> cameraNames) {
	QList<QFile*> saveFiles;
	for (int i = 0; i < m_datasetConfig->numCameras; i++) {
		QDir dir;
		dir.mkpath(dataFolder + "/" + cameraNames[i]);
		QFile *file = new QFile(dataFolder + "/" + cameraNames[i] +"/annotations.csv");
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
		 for (int i = 0; i <num_columns; i++) {
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
	/*for (auto& imgSet : m_imgSets) {
		for (int cam = 0; cam < m_numCameras; cam++) {
			QTextStream stream(saveFiles[cam]);
			stream << imgSet->frames[cam]->imagePath.split("/").last() << ",";	//decide on what should be in this path (look at DLC2.2)
			for (int i = 0; i < m_keypointNameList.size(); i++) {
				Keypoint *keypoint =  imgSet->frames[cam]->keypointMap[m_entityNameList[i] + "/" + m_keypointNameList[i]];
				if (keypoint->state() == NotAnnotated) {
					stream << ",," << 0 << ",";
				}
				else if (keypoint->state() == Annotated) {
					stream << keypoint->rx() << "," << keypoint->ry() << ",";
					stream << 1 << ",";
				}
				else if (keypoint->state() == Reprojected) {
					stream << keypoint->rx() << "," << keypoint->ry() << ",";
					stream << 2 << ",";
				}
				else {
					stream << ",," << 3 << ",";
				}
			}
			stream << "\n";
		}
	}
	for (int i = 0; i < m_numCameras; i++) {
		saveFiles[i]->close();
	}*/
}
