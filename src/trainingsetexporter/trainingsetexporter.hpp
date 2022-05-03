/*******************************************************************************
 * File:			  trainingsetexporter.hpp
 * Created: 	  18. August 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

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
		void copiedFrameSet(int counter, int total, const QString &setName);
		void exportFinished();

	public slots:
		void exportTrainingsetSlot(ExportConfig exportConfig);
		void exportCanceledSlot();

	private:
		QList<DatasetExportItem> &m_datasetExportItems;
		QString m_primaryCamera;
		bool m_exportCanceled = false;

		void addInfo(json &j);
		void addCategories(json &j, ExportConfig &exportConfig);
		void addCalibration(json &j, ExportConfig &exportConfig);
		void copyCalibrationParams(ExportConfig &exportConfig);
		QList<ExportFrameSet> loadAllFrameSets(ExportConfig &exportConfig);
		void addFrameSetsToJSON(ExportConfig &exportConfig,
					const QList<ExportFrameSet> &frameSets, json & j);
		QMap<QString, bool> makeMapfromPairs(
					const QList<QPair<QString, bool>> &pairs);
		void copyFrames(ExportConfig &exportConfig,
					const QList<ExportFrameSet> &frameSets, const QString &setName);
		bool checkCalibrationParamPaths(ExportConfig &exportConfig);

	private slots:


};

#endif
