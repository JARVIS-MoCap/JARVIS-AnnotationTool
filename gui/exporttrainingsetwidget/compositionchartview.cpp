/*------------------------------------------------------------
 *  compositionchartview.cpp
 *  Created: 12. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "compositionchartview.hpp"


CompositionChartView::CompositionChartView(QList<QString> names, QList<int> frameNumbers) : m_names(names), m_frameNumbers(frameNumbers) {
	this->setRenderHint(QPainter::Antialiasing);
	this->setStyleSheet("background-color: transparent");
	update();
}

void CompositionChartView::update() {
	chart = new QChart();
	this->setChart(chart);
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));

	QPieSeries *series = new QPieSeries();
	series->append("Jane", 0);
  series->append("Jane", 1);
  series->append("Joe", 1);
  series->append("Andy", 2);
  series->append("Barbara", 3);
  series->append("Axel", 4);
	connect(series, &QPieSeries::hovered, this, &CompositionChartView::onHoverSlot);

  chart->addSeries(series);
  chart->setTitle(" ");
	chart->setBackgroundVisible(false);
	chart->legend()->hide();

}

void CompositionChartView::onHoverSlot(QPieSlice *slice, bool state) {
	if (state == true) {
		slice->setExploded();
		chart->setTitle(slice->label());

	}
	else {
		slice->setExploded(false);
		chart->setTitle(" ");

	}

}
