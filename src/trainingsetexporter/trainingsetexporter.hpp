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

#include <QRunnable>

class TrainingSetExporter : public QObject {
	Q_OBJECT

	public:
		explicit TrainingSetExporter(QList<DatasetExportItem> &datasetExportItems);

	signals:

	public slots:
		void exportTrainingsetSlot();

	private:
		QList<DatasetExportItem> &m_datasetExportItems;


	private slots:


};

#endif
