/*------------------------------------------------------------
 *  annotationstatisticschartview.cpp
 *  Created: 12. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "annotationstatisticschartview.hpp"


AnnotationStatisticsChartView::AnnotationStatisticsChartView() {
	this->setRenderHint(QPainter::Antialiasing);
	this->setStyleSheet("background-color: transparent");

	chart = new QChart();
	this->setChart(chart);
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));
	chart->setTitleFont(QFont("Sans Serif", 12, QFont::Bold));
	chart->setTitle("");
	chart->setMargins(QMargins(0,0,0,0));
	chart->legend()->setFont(QFont("Sans Serif", 10));
	//chart->setBackgroundVisible(false);
	//chart->legend()->hide();

	update();
}


void AnnotationStatisticsChartView::update(QList<DatasetExportItem> *datasetExportItems, int index) {
	chart->removeAllSeries();
	QPieSeries *series = new QPieSeries();
	series->setPieSize(0.85);

	if (datasetExportItems != nullptr && datasetExportItems->size() != 0) {
		if (index == -1) {
			AnnotationCount totalAnnotationCount;
			for (const auto& exportItem : *datasetExportItems) {
				totalAnnotationCount = totalAnnotationCount + exportItem.annotationCount;
			}
			series->append("Annotated", totalAnnotationCount.annotated);
			series->append("Reprojected", totalAnnotationCount.reprojected);
			series->append("Not Annotated", totalAnnotationCount.notAnnotated);
		}
		else {
			series->append("Annotated", (*datasetExportItems)[index].annotationCount.annotated);
			series->append("Reprojected", (*datasetExportItems)[index].annotationCount.reprojected);
			series->append("Not Annotated", (*datasetExportItems)[index].annotationCount.notAnnotated);
		}
		QPieSlice *slice = series->slices().at(0);
		slice->setPen(QPen(QColor(34, 36, 40), 2));
		slice->setBrush(QColor(100,164,32));
		slice = series->slices().at(1);
		slice->setPen(QPen(QColor(34, 36, 40), 2));
		slice->setBrush(QColor(32,100,164));
		slice = series->slices().at(2);
		slice->setPen(QPen(QColor(34, 36, 40), 2));
		slice->setBrush(QColor(100,32,164));
	}
	else {
		series->append("", 1);
		series->append("", 1);
		series->append("", 1);
		QPieSlice *slice = series->slices().at(0);
		slice->setPen(QPen(QColor(34, 36, 40), 2));
		slice->setBrush(QColor(127, 127, 127));
		slice = series->slices().at(1);
		slice->setPen(QPen(QColor(34, 36, 40), 2));
		slice->setBrush(QColor(127, 127, 127));
		slice = series->slices().at(2);
		slice->setPen(QPen(QColor(34, 36, 40), 2));
		slice->setBrush(QColor(127, 127, 127));
	}

  chart->addSeries(series);
}
