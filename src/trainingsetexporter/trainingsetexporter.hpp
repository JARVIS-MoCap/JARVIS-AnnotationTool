/*****************************************************************
	* File:			  trainingsetexporter.hpp
	* Created: 	  18. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef TRAININGSETEXPORTER_H
#define TRAININGSETEXPORTER_H

#include "globals.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include <QRunnable>

class TrainingSetExporter : public QObject {
	Q_OBJECT

	public:
		explicit TrainingSetExporter(QList<DatasetExportItem> &datasetExportItems);

	signals:

	public slots:
		void exportTrainingsetSlot(ExportConfig exportConfig);

	private:
		QList<DatasetExportItem> &m_datasetExportItems;
		QString m_primaryCamera;

		void addInfo(json &j);
		void addCategories(json &j, ExportConfig &exportConfig);
		void addCalibration(json &j, ExportConfig &exportConfig);
		void copyCalibrationParams(ExportConfig &exportConfig);
		QList<ExportFrameSet> loadAllFrameSets(ExportConfig &exportConfig);
		void addFrameSetsToJSON(ExportConfig &exportConfig, const QList<ExportFrameSet> &frameSets, json & j);
		QMap<QString, bool> makeMapfromPairs(const QList<QPair<QString, bool>> &pairs);
		void copyFrames(ExportConfig &exportConfig, const QList<ExportFrameSet> &frameSets, const QString &setName);
	private slots:


};

#endif
