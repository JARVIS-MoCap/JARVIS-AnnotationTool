/*****************************************************************
 * File:			 trainingsetexporter.cpp
 * Created: 	 18. August 2021
 * Author:		 Timo Hueser
 * Contact: 	 timo.hueser@gmail.com
 * Copyright:  2021 Timo Hueser
 * License:    GPL v3.0
 *****************************************************************/

#include "trainingsetexporter.hpp"
#include <fstream>
#include <iomanip>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDirIterator>
#include <QThreadPool>

TrainingSetExporter::TrainingSetExporter(QList<DatasetExportItem> &datasetExportItems) :
			m_datasetExportItems(datasetExportItems) {

}

void TrainingSetExporter::exportTrainingsetSlot(ExportConfig exportConfig) {
	std::cout << "Exporting TrainingSet" << std::endl;
  QDir dir;
	dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName);
	dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName + "/annotations");

	if (exportConfig.trainingSetType == "3D") {
		copyCalibrationParams(exportConfig);
	}

	json trainingSet;
	addInfo(trainingSet);
	addCategories(trainingSet, exportConfig);
	if (exportConfig.trainingSetType == "3D") {
		addCalibration(trainingSet, exportConfig);
	}

	json validationSet;
	addInfo(validationSet);
	addCategories(validationSet, exportConfig);
	if (exportConfig.trainingSetType == "3D") {
		addCalibration(validationSet, exportConfig);
	}


	QList<ExportFrameSet> exportFrameSets = loadAllFrameSets(exportConfig);
	addFrameSetsToJSON(exportFrameSets, trainingSet);
	copyFrames(exportConfig, exportFrameSets, "train");


	std::ofstream trainStream((exportConfig.savePath + "/" + exportConfig.trainingSetName + "/annotations/instances_train.json").toStdString());
	trainStream << trainingSet << std::endl;
	std::ofstream valStream((exportConfig.savePath + "/" + exportConfig.trainingSetName + "/annotations/instances_val.json").toStdString());
	valStream << validationSet << std::endl;

}


void TrainingSetExporter::addInfo(json &j) {
	j["info"]["description"] = "";	//TODO: do I want to add the option to add a description somewhere?
	j["info"]["url"] = "";
	j["info"]["versoin"] = "1.0";
	j["info"]["year"] = QDate::currentDate().year();
	j["info"]["contributor"] = "";
	j["info"]["date_created"] = QDate::currentDate().toString(Qt::ISODate).toStdString();
	j["licenses"] = {{{"id", 1}, {"name", ""}, {"url", ""}}};
}


void TrainingSetExporter::addCategories(json &j, ExportConfig &exportConfig) {
	j["categories"] = json::array();
	int num_keypoints = 0;
	for (const auto &keypoint : exportConfig.keypointsList) {
		if (keypoint.second) num_keypoints++;
	}
	int counter = 0;
	for (const auto &entity : exportConfig.entitiesList) {
		if (entity.second) {
			j["categories"].push_back({{"id", counter++}, {"name", entity.first.toStdString()}, {"supercategory", "None"}, {"num_keypoints", num_keypoints}});
		}
	}
}


void TrainingSetExporter::addCalibration(json &j, ExportConfig &exportConfig) {
	QList<QString> cameras;
	for (auto & fileName : QDir(exportConfig.intrinsicsPath).entryList(QDir::Files | QDir::NoDotAndDotDot)) {
		cameras.append(fileName.remove("Intrinsics_").remove(".yaml"));
	}

	QList<QString> extrinsicsPairs = QDir(exportConfig.extrinsicsPath).entryList(QDir::Files | QDir::NoDotAndDotDot);

	QString primaryCamera;
	for (const auto &path : extrinsicsPairs) {
		QString pairPrimary;
		int minIndex = 9999;
		for (const auto &cam : cameras) {
			int index = path.indexOf(cam);
			if (path.indexOf(cam) != -1) {
				if (index < minIndex) {
					pairPrimary = cam;
					minIndex = index;
				}
			}
		}
		if (primaryCamera == "") {
			primaryCamera = pairPrimary;
		}
		else if (primaryCamera != pairPrimary) {
			std::cout << "INVALID CALIBRATION SET" << std::endl;
			return;
		}
	}
	j["calibration"]["primary_camera"] = primaryCamera.toStdString();
	j["calibration"]["intrinsics"] = json::object();
	j["calibration"]["extrinsics"] = json::object();
	for (const auto & cam : cameras) {
		j["calibration"]["intrinsics"].push_back({cam.toStdString(), "calib_params/Intrinsics/Intrinsics_" + cam.toStdString() + ".yaml"});
		if (cam != primaryCamera) {
			j["calibration"]["extrinsics"].push_back({cam.toStdString(), "calib_params/Extrinsics/Extrinsics_" + primaryCamera.toStdString() + "_" + cam.toStdString() + ".yaml"});
		}
	}
}

void TrainingSetExporter::copyCalibrationParams(ExportConfig &exportConfig) {
	QDir dir;
	dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName + "/calib_params/Intrinsics");
	for (auto & fileName : QDir(exportConfig.intrinsicsPath).entryList(QDir::Files | QDir::NoDotAndDotDot)) {
		QFile::copy(exportConfig.intrinsicsPath + "/" + fileName, exportConfig.savePath + "/" + exportConfig.trainingSetName + "/calib_params/Intrinsics/" + fileName);
	}
	dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName + "/calib_params/Extrinsics");
	for (auto & fileName : QDir(exportConfig.extrinsicsPath).entryList(QDir::Files | QDir::NoDotAndDotDot)) {
		QFile::copy(exportConfig.extrinsicsPath + "/" + fileName, exportConfig.savePath + "/" + exportConfig.trainingSetName + "/calib_params/Extrinsics/" + fileName);
	}
}

QList<ExportFrameSet> TrainingSetExporter::loadAllFrameSets(ExportConfig &exportConfig) {
	QList<ExportFrameSet> frameSets;
	QMap<QString, bool> entitiesSaveMap = makeMapfromPairs(exportConfig.entitiesList);
	QMap<QString, bool> keypointsSaveMap = makeMapfromPairs(exportConfig.keypointsList);
	for (const auto &exportItem : m_datasetExportItems) {
		for (const auto & subSet : exportItem.subSets) {
			QMap<QString,QList<QPair<QString,QList<QPointF>>>> keypointsMap;
			if (subSet.second) {
				QList<QString> cameras = QDir(exportItem.basePath + "/" + subSet.first).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
				for (const auto & camera : cameras) {
					QList<QPair<QString,QList<QPointF>>> framesList;
					QFile *file = new QFile(exportItem.basePath + "/" + subSet.first + "/" + camera + "/annotations.csv");
					if (!file->open(QIODevice::ReadOnly)) {
						std::cout << (exportItem.basePath + "/" + subSet.first + "/" + camera + "/annotations.csv").toStdString() << std::endl;
						std::cout << "Error reading file!" << std::endl;
					}
					else {
						file->readLine(); //Scorer
						QList<QByteArray> entityNames = file->readLine().split(',');
						QList<QByteArray> keypointNames = file->readLine().split(',');
						file->readLine(); //Coords
						while (!file->atEnd()) {
							QList<QByteArray> cells = file->readLine().split(',');
							QPair<QString,QList<QPointF>> keypoints;
							keypoints.first = cells[0];
							for (int i = 1; i < cells.size()-2; i += 3) {
								if (entitiesSaveMap[entityNames[i]] && keypointsSaveMap[keypointNames[i]]) {
									QPointF keypoint = QPointF(cells[i].toFloat(), cells[i+1].toFloat());
									keypoints.second.append(keypoint);
								}
							}
							framesList.append(keypoints);
						}
						keypointsMap[camera] = framesList;
					}
				}
				for (int i = 0; i < keypointsMap[cameras[0]].size(); i++) {
					ExportFrameSet frameSet;
					frameSet.originalPath = exportItem.basePath + "/" + subSet.first;
					frameSet.basePath = exportItem.basePath.split("/").takeLast() + "/" + subSet.first;
					frameSet.cameras = cameras;
					QMap<QString, QPair<QString,QList<QPointF>>> keypoints;
					for (const auto camera : cameras) {
						keypoints[camera] = keypointsMap[camera][i];
					}
					frameSet.keyPoints = keypoints;
					frameSets.append(frameSet);
				}
			}
		}
	}
	return frameSets;
}


QMap<QString, bool> TrainingSetExporter::makeMapfromPairs(const QList<QPair<QString, bool>> &pairs) {
	QMap<QString, bool> map;
	for (const auto & pair : pairs) {
		map[pair.first] = pair.second;
	}
	return map;
}


void TrainingSetExporter::addFrameSetsToJSON(const QList<ExportFrameSet> &frameSets, json & j) {
	int id = 0;
	j["annotations"] = json::array();
	j["images"] = json::array();

	for (const auto &frameSet : frameSets) {
		for (const auto &camera : frameSet.cameras) {
			float x_min = -1;
			float x_max = -1;
			float y_min = -1;
			float y_max = -1;
			std::vector<int> keypoints;
			for (const auto &keypoint : frameSet.keyPoints[camera].second) {
				keypoints.push_back(keypoint.x());
				keypoints.push_back(keypoint.y());
				keypoints.push_back(1);

				if (x_min == -1) x_min = keypoint.x();
				else if (keypoint.x() < x_min) x_min = keypoint.x();
				if (y_min == -1) y_min = keypoint.y();
				else if (keypoint.y() < y_min) y_min = keypoint.y();
				if (x_max == -1) x_max = keypoint.x();
				else if (keypoint.x() > x_max) x_max = keypoint.x();
				if (y_max == -1) y_max = keypoint.y();
				else if (keypoint.y() > y_max) y_max = keypoint.y();
			}
			float x_size = x_max-x_min;
			float y_size = y_max-y_min;


			j["annotations"].push_back({
				{"area", 0},
				{"iscrowd", 0},
				{"image_id", id},
				{"segmentation", json::array()},
				{"bbox", {x_min, y_min, x_size, y_size}},
				{"num_keypoints", frameSet.keyPoints[camera].second.size()},
				{"keypoints", keypoints},
				{"category_id", 1},
				{"id", id}
			});

			j["images"].push_back({
				{"id", id++},
				{"file_name", (frameSet.basePath + "/" + camera + "/" + frameSet.keyPoints[camera].first).toStdString()},
				{"width", 1280},
				{"height", 1024},
				{"date_captured", ""},
				{"license", 1},
				{"coco_url", ""},
				{"flickr_url", ""}
			});
		}
	}
}

void TrainingSetExporter::copyFrames(ExportConfig &exportConfig, const QList<ExportFrameSet> &frameSets, const QString &setName) {
	QDir dir;
	for (const auto &frameSet : frameSets) {
		for (const auto &camera : frameSet.cameras) {
			dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName + "/" + setName + "/" + frameSet.basePath + "/" + camera);
			QString originalPath = frameSet.originalPath + "/" + camera + "/" + frameSet.keyPoints[camera].first;
			QString newPath = exportConfig.savePath + "/" + exportConfig.trainingSetName + "/" + setName + "/" + frameSet.basePath + "/" + camera + "/" + frameSet.keyPoints[camera].first;
			QFile::copy(originalPath, newPath);
		}
	}
}
