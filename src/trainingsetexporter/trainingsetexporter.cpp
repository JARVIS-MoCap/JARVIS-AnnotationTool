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
	// create an empty structure (null)
json j;
addInfo(j);
addCategories(j, exportConfig);
if (exportConfig.trainingSetType == "3D") {
	addCalibration(j, exportConfig);
}

std::ofstream o("test.json");
o << std::setw(4) << j << std::endl;
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
		std::cout << cam.toStdString() << std::endl;
		j["calibration"]["intrinsics"].push_back({cam.toStdString(), "calib_params/Intrinsics/Intrinsics_" + cam.toStdString() + ".yaml"});
		if (cam != primaryCamera) {
			j["calibration"]["extrinsics"].push_back({cam.toStdString(), "calib_params/Extrinsics/Extrinsics_" + primaryCamera.toStdString() + "_" + cam.toStdString() + ".yaml"});
		}
	}
}
