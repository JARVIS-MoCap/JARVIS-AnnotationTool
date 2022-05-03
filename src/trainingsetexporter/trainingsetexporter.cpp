/*******************************************************************************
 * File:			  trainingsetexporter.cpp
 * Created: 	  18. August 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/

#include "trainingsetexporter.hpp"
#include <fstream>
#include <iomanip>

#include <algorithm>
#include <random>
#include <chrono>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDirIterator>
#include <QThreadPool>
#include <QErrorMessage>


TrainingSetExporter::TrainingSetExporter(
			QList<DatasetExportItem> &datasetExportItems) :
			m_datasetExportItems(datasetExportItems) {
}

void TrainingSetExporter::exportTrainingsetSlot(ExportConfig exportConfig) {
	std::cout << "Exporting TrainingSet" << std::endl;
  QDir dir;
	dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName);
	dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName +
				"/annotations");

	if (exportConfig.trainingSetType == "3D") {
		if (checkCalibrationParamPaths(exportConfig)) {
			copyCalibrationParams(exportConfig);
		}
		else {
			QErrorMessage* errorMsg = new QErrorMessage();
			errorMsg->showMessage("Check if all datasets contain correct CalibrationParameters!");
		}
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

	if (exportConfig.shuffleBeforeSplit) {
		auto rng = std::default_random_engine {};
		if (!exportConfig.useRandomShuffleSeed)  {
			rng.seed(exportConfig.shuffleSeed);
		}
		else {
			rng.seed(std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch()).count());
		}
		std::shuffle(std::begin(exportFrameSets), std::end(exportFrameSets), rng);
	}

	QList<ExportFrameSet> trainingFrameSets;
	QList<ExportFrameSet> validationFrameSets;

	for (int i = 0; i < exportFrameSets.size(); i++) {
		if (i < exportConfig.validationFraction*exportFrameSets.size()) {
			validationFrameSets.append(exportFrameSets[i]);
		}
		else {
			trainingFrameSets.append(exportFrameSets[i]);
		}
	}


	addFrameSetsToJSON(exportConfig, trainingFrameSets, trainingSet);
	copyFrames(exportConfig, trainingFrameSets, "train");

	addFrameSetsToJSON(exportConfig, validationFrameSets, validationSet);
	copyFrames(exportConfig, validationFrameSets, "val");

	std::ofstream trainStream((exportConfig.savePath + "/" +
				exportConfig.trainingSetName +
				"/annotations/instances_train.json").toStdString());
	trainStream << trainingSet << std::endl;
	std::ofstream valStream((exportConfig.savePath + "/" +
				exportConfig.trainingSetName +
				"/annotations/instances_val.json").toStdString());
	valStream << validationSet << std::endl;
	if (m_exportCanceled) {
		m_exportCanceled = false;
		return;
	}
	emit exportFinished();
}


void TrainingSetExporter::addInfo(json &j) {
	j["info"]["description"] = "";
	j["info"]["url"] = "";
	j["info"]["versoin"] = "1.0";
	j["info"]["year"] = QDate::currentDate().year();
	j["info"]["contributor"] = "";
	j["info"]["date_created"] = QDate::currentDate().toString(
				Qt::ISODate).toStdString();
	j["licenses"] = {{{"id", 1}, {"name", ""}, {"url", ""}}};
}


void TrainingSetExporter::addCategories(json &j, ExportConfig &exportConfig) {
	j["categories"] = json::array();
	j["keypoint_names"] = json::array();
	int num_keypoints = 0;
	for (const auto &keypoint : exportConfig.keypointsList) {
		if (keypoint.second) {
			num_keypoints++;
			j["keypoint_names"].push_back(keypoint.first.toStdString());
		}
	}
	int counter = 0;
	for (const auto &entity : exportConfig.entitiesList) {
		if (entity.second) {
			j["categories"].push_back({{"id", counter++},
						{"name", entity.first.toStdString()}, {"supercategory", "None"},
						{"num_keypoints", num_keypoints}});
		}
	}
	j["skeleton"] = json::array();

	for (const auto &skeletonComp : exportConfig.skeleton) {
		j["skeleton"].push_back({
					{"name", skeletonComp.name.toStdString()},
					{"keypointA", skeletonComp.keypointA.toStdString()},
					{"keypointB", skeletonComp.keypointB.toStdString()},
					{"length", skeletonComp.length}});
	}
}


void TrainingSetExporter::addCalibration(json &j, ExportConfig &exportConfig) {
	QList<QString> cameras;
	for (auto & fileName : QDir(m_datasetExportItems[0].basePath + "/CalibrationParameters").entryList(QDir::Files
				| QDir::NoDotAndDotDot)) {
		cameras.append(fileName.remove(".yaml"));
	}
	QList<QString> datasetNames;
	for (const auto& exportItem : m_datasetExportItems) {
		datasetNames.append(exportItem.name);
	}
	for (const auto& datasetName : datasetNames) {
		j["calibrations"][datasetName.toStdString()] = json::object();
		for (const auto & cam : cameras) {
			j["calibrations"][datasetName.toStdString()].push_back({cam.toStdString(),
						"calib_params/" + datasetName.toStdString() + "/" + cam.toStdString() + ".yaml"});
		}
	}
}


void TrainingSetExporter::copyCalibrationParams(ExportConfig &exportConfig) {
	for (const auto& exportItem : m_datasetExportItems) {
		QDir dir;
		dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName +
		"/calib_params/" + exportItem.name);
		for (auto & fileName : QDir(exportItem.basePath + "/CalibrationParameters").entryList(
					QDir::Files | QDir::NoDotAndDotDot)) {
			QFile::copy(exportItem.basePath + "/CalibrationParameters/" + fileName,
						exportConfig.savePath + "/" + exportConfig.trainingSetName +
						"/calib_params/" + exportItem.name + "/" + fileName);
		}
	}
}


bool TrainingSetExporter::checkCalibrationParamPaths(ExportConfig &exportConfig) {
	int num_calibs = -1;
	for (const auto& exportItem : m_datasetExportItems) {
		QList<QString> calibFiles = QDir(exportItem.basePath + "/CalibrationParameters").entryList(
					QDir::Files | QDir::NoDotAndDotDot);
		if (num_calibs == -1) {
			num_calibs = calibFiles.size();
		}
		else {
			if (num_calibs != calibFiles.size()) {
				return false;
			}
		}
	}
	if (num_calibs == 0) return false;
	return true;
}


QList<ExportFrameSet> TrainingSetExporter::loadAllFrameSets(
			ExportConfig &exportConfig) {
	QList<ExportFrameSet> frameSets;
	QMap<QString, bool> entitiesSaveMap =
				makeMapfromPairs(exportConfig.entitiesList);
	QMap<QString, bool> keypointsSaveMap =
				makeMapfromPairs(exportConfig.keypointsList);
	for (const auto &exportItem : m_datasetExportItems) {
		for (const auto & subSet : exportItem.subSets) {
			QMap<QString,QList<QPair<QString,QList<ExportKeypoint>>>> keypointsMap;
			if (subSet.second) {
				QList<QString> cameras =
							QDir(exportItem.basePath + "/" + subSet.first).entryList(
							QDir::AllDirs | QDir::NoDotAndDotDot);
				for (const auto & camera : cameras) {
					QList<QPair<QString,QList<ExportKeypoint>>> framesList;
					QFile *file = new QFile(exportItem.basePath + "/" + subSet.first + "/"
								+ camera + "/annotations.csv");
					if (!file->open(QIODevice::ReadOnly)) {
						std::cout << (exportItem.basePath + "/" + subSet.first + "/" +
									camera + "/annotations.csv").toStdString() << std::endl;
						std::cout << "Error reading file!" << std::endl;
					}
					else {
						file->readLine(); //Scorer
						QList<QByteArray> entityNames = file->readLine().split(',');
						QList<QByteArray> keypointNames = file->readLine().split(',');
						file->readLine(); //Coords
						while (!file->atEnd()) {
							QList<QByteArray> cells = file->readLine().split(',');
							QPair<QString,QList<ExportKeypoint>> keypoints;
							keypoints.first = cells[0];
							for (int i = 1; i < cells.size()-2; i += 3) {
								if (entitiesSaveMap[entityNames[i]] &&
											keypointsSaveMap[keypointNames[i]]) {
									ExportKeypoint keypoint;
									keypoint.point = QPointF(cells[i].toFloat(),
												cells[i+1].toFloat());
									keypoint.state = static_cast<KeypointState>(
												cells[i+2].toInt());
									keypoints.second.append(keypoint);
								}
							}
							framesList.append(keypoints);
						}
						keypointsMap[camera] = framesList;
					}
					file->close();
				}
				for (int i = 0; i < keypointsMap[cameras[0]].size(); i++) {
					ExportFrameSet frameSet;
					frameSet.datasetName = exportItem.name;
					frameSet.originalPath = exportItem.basePath + "/" + subSet.first;
					frameSet.basePath = exportItem.basePath.split("/").takeLast() + "/" +
								subSet.first;
					frameSet.cameras = cameras;
					QMap<QString, QPair<QString,QList<ExportKeypoint>>> keypoints;
					for (const auto camera : cameras) {
						keypoints[camera] = keypointsMap[camera][i];
					}
					frameSet.keypoints = keypoints;
					frameSets.append(frameSet);
				}
			}
		}
	}
	return frameSets;
}


QMap<QString, bool> TrainingSetExporter::makeMapfromPairs(
			const QList<QPair<QString, bool>> &pairs) {
	QMap<QString, bool> map;
	for (const auto & pair : pairs) {
		map[pair.first] = pair.second;
	}
	return map;
}


void TrainingSetExporter::addFrameSetsToJSON(ExportConfig &exportConfig,
			const QList<ExportFrameSet> &frameSets, json & j) {
	int id = 0;
	j["annotations"] = json::array();
	j["images"] = json::array();
	QMap<QString, QList<int>> frameSetIndexMap;
	QMap<QString, QString> calibDatasetNameMap;
	for (const auto &frameSet : frameSets) {
		for (const auto &camera : frameSet.cameras) {
			float x_min = -1;
			float x_max = -1;
			float y_min = -1;
			float y_max = -1;
			std::vector<int> keypoints;
			for (const auto &keypoint : frameSet.keypoints[camera].second) {
				keypoints.push_back(keypoint.point.x());
				keypoints.push_back(keypoint.point.y());
				keypoints.push_back(1);

				if (keypoint.state != NotAnnotated && keypoint.state != Suppressed) {
					if (x_min == -1) x_min = keypoint.point.x();
					else if (keypoint.point.x() < x_min) x_min = keypoint.point.x();
					if (y_min == -1) y_min = keypoint.point.y();
					else if (keypoint.point.y() < y_min) y_min = keypoint.point.y();
					if (x_max == -1) x_max = keypoint.point.x();
					else if (keypoint.point.x() > x_max) x_max = keypoint.point.x();
					if (y_max == -1) y_max = keypoint.point.y();
					else if (keypoint.point.y() > y_max) y_max = keypoint.point.y();
				}
			}

			j["images"].push_back({
				{"id", id},
				{"file_name", (frameSet.basePath + "/" + camera + "/" +
							frameSet.keypoints[camera].first).toStdString()},
				{"width", 1280},	//TODO: Make this not be hardcoded!
				{"height", 1024},
				{"date_captured", ""},
				{"license", 1},
				{"coco_url", ""},
				{"flickr_url", ""}
			});

			if (x_min != -1) {
				float x_size = x_max-x_min;
				float y_size = y_max-y_min;

				//TODO: Make this work with multiple entities
				j["annotations"].push_back({
					{"area", 0},
					{"iscrowd", 0},
					{"image_id", id},
					{"segmentation", json::array()},
					{"bbox", {x_min, y_min, x_size, y_size}},
					{"num_keypoints", frameSet.keypoints[camera].second.size()},
					{"keypoints", keypoints},
					{"category_id", 1},
					{"id", id}
				});
			}

			if (exportConfig.trainingSetType == "3D") {
				if (frameSetIndexMap.contains(frameSet.basePath + "/" +
							frameSet.keypoints[camera].first.split(".").takeFirst())) {
					frameSetIndexMap[frameSet.basePath + "/" +
								frameSet.keypoints[camera].first.split(".").takeFirst()].
								append(id);
				}
				else {
					frameSetIndexMap[frameSet.basePath + "/" +
								frameSet.keypoints[camera].first.split(".").takeFirst()] = {id};
					calibDatasetNameMap[frameSet.basePath + "/" +
								frameSet.keypoints[camera].first.split(".").takeFirst()] = frameSet.datasetName;
				}
			}
			id++;
		}
	}
	if (exportConfig.trainingSetType == "3D") {
		for(const auto &frameName : frameSetIndexMap.keys()) {
			j["framesets"][frameName.toStdString()]["datasetName"] = calibDatasetNameMap[frameName].toStdString();
			j["framesets"][frameName.toStdString()]["frames"] = json::array();
			for (const auto &id : frameSetIndexMap[frameName]) {
				j["framesets"][frameName.toStdString()]["frames"].push_back(id);
			}
		}
	}
}


void TrainingSetExporter::copyFrames(ExportConfig &exportConfig,
			const QList<ExportFrameSet> &frameSets, const QString &setName) {
	if (m_exportCanceled) {
		return;
	}
	QDir dir;
	std::cout << "FramesSets: " << frameSets.size() << std::endl;
	int frameSetCounter = 1;
	for (const auto &frameSet : frameSets) {
		for (const auto &camera : frameSet.cameras) {
			dir.mkpath(exportConfig.savePath + "/" + exportConfig.trainingSetName +
						"/" + setName + "/" + frameSet.basePath + "/" + camera);
			QString originalPath = frameSet.originalPath + "/" + camera + "/" +
						frameSet.keypoints[camera].first;
			QString newPath = exportConfig.savePath + "/" +
						exportConfig.trainingSetName + "/" + setName + "/" +
						frameSet.basePath + "/" + camera + "/" +
						frameSet.keypoints[camera].first;
			QFile::copy(originalPath, newPath);
		}
		if (m_exportCanceled) {
			return;
		}
		emit copiedFrameSet(frameSetCounter++, frameSets.size(), setName);
	}
}

void TrainingSetExporter::exportCanceledSlot() {
	m_exportCanceled = true;
}
