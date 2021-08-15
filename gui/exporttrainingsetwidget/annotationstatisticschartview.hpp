/*------------------------------------------------------------
 *  annotationstatisticschartview.hpp
 *  Created: 12. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef ANNOTATIONSTATISTICSCHARTVIEW_H
#define ANNOTATIONSTATISTICSCHARTVIEW_H

#include "globals.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QtCharts>


class AnnotationStatisticsChartView : public QChartView {
	Q_OBJECT
	public:
		explicit AnnotationStatisticsChartView();
		void update(QList<DatasetExportItem> *datasetExportItems = nullptr, int index = -1);

	private:
		QChart *chart;

	private slots:

};

#endif
