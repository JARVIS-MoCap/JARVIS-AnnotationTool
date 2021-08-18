/*****************************************************************
 * File:			 trainingsetexporter.cpp
 * Created: 	 18. August 2021
 * Author:		 Timo Hueser
 * Contact: 	 timo.hueser@gmail.com
 * Copyright:  2021 Timo Hueser
 * License:    GPL v3.0
 *****************************************************************/

#include "trainingsetexporter.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDirIterator>
#include <QThreadPool>

TrainingSetExporter::TrainingSetExporter(QList<DatasetExportItem> &datasetExportItems) :
			m_datasetExportItems(datasetExportItems) {

}

void TrainingSetExporter::exportTrainingsetSlot() {
	std::cout << "Exporting TrainingSet" << std::endl;
}
