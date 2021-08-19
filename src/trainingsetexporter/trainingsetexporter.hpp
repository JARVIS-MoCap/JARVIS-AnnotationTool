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

		void addInfo(json &j);
		void addCategories(json &j, ExportConfig &exportConfig);
		void addCalibration(json &j, ExportConfig &exportConfig);

	private slots:


};

#endif
