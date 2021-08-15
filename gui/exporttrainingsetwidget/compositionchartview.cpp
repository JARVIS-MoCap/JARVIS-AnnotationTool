/*------------------------------------------------------------
 *  compositionchartview.cpp
 *  Created: 12. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "compositionchartview.hpp"


CompositionChartView::CompositionChartView() {
	this->setRenderHint(QPainter::Antialiasing);
	this->setStyleSheet("background-color: transparent");
	chart = new QChart();
	this->setChart(chart);
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));
	chart->setTitleFont(QFont("Sans Serif", 14, QFont::Bold));
  chart->setTitle("Trainingset Composition");
	chart->setBackgroundVisible(false);
	chart->setMargins(QMargins(0,0,0,0));
	chart->legend()->hide();
	update();
}

void CompositionChartView::update(QList<DatasetExportItem> *datasetExportItems) {
	chart->removeAllSeries();
	m_series = new QPieSeries();
	m_series->setPieSize(0.85);

	QList<QColor> colors = {QColor("#332288"), QColor("#3cb3ee"), QColor("#2baa95"), QColor("#1e773c"), QColor("#999926"), QColor("#ddc137"), QColor("#a3181b"), QColor("#882255"), QColor("#aa2b95")};



	int i = 0;
	if (datasetExportItems != nullptr && datasetExportItems->size() != 0) {
		for (const auto &exportItem : *datasetExportItems) {
			m_series->append(exportItem.name, exportItem.frameCount);
			QPieSlice *slice = m_series->slices().at(i);
			slice->setPen(QPen(QColor(56,58,63), 4));
			slice->setBrush(colors[i%colors.size()]);
			i++;
		}
		connect(m_series, &QPieSeries::hovered, this, &CompositionChartView::onHoverSlot);
 	}

	else {
		m_series->append("", 1);
		m_series->append("", 1);
		m_series->append("", 1);
		QPieSlice *slice = m_series->slices().at(0);
		slice->setPen(QPen(QColor(56,58,63), 4));
		slice->setBrush(QColor(127, 127, 127));
		slice = m_series->slices().at(1);
		slice->setPen(QPen(QColor(56,58,63), 4));
		slice->setBrush(QColor(127, 127, 127));
		slice = m_series->slices().at(2);
		slice->setPen(QPen(QColor(56,58,63), 4));
		slice->setBrush(QColor(127, 127, 127));
	}

  chart->addSeries(m_series);
}

void CompositionChartView::onHoverSlot(QPieSlice *slice, bool state) {
	if (state == true) {
		slice->setExploded();
		emit hoverStarted(m_series->slices().indexOf(slice));

	}
	else {
		slice->setExploded(false);
		emit hoverEnded();
	}

}
