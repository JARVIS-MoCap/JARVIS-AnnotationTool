/*****************************************************************
	* File:			  reprojectionchartview.cpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "reprojectionchartview.hpp"


ReprojectionChartView::ReprojectionChartView() {
}


void ReprojectionChartView::update(std::vector<double> *reproErrors, int selectedIndex) {
	if (reproErrors != nullptr) {
		m_reproErrors = reproErrors;
	}
	if (m_reproErrors == nullptr) return;
	QChart *chart = new QChart();
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));
	chart->legend()->setVisible(false);
	if (selectedIndex != -1) {
		chart->setTitle(Dataset::dataset->bodypartsList()[selectedIndex]);
	}
	else {
		chart->setTitle(" ");
	}
	float maxValue = *std::max_element(m_reproErrors->begin(), m_reproErrors->end());

	QValueAxis *axisY = new QValueAxis();
	axisY->setRange(0,maxValue);

	chart->addAxis(axisY, Qt::AlignLeft);
	this->setChart(chart);
	int count = 0;
	m_barSeriesList.clear();
	for (const auto& error : *m_reproErrors) {
		QBarSeries *series = new QBarSeries();
		m_barSeriesList.append(series);
		connect(series, &QBarSeries::hovered, this, &ReprojectionChartView::onHoverSlot);
		QBarSet * set = new QBarSet("Test");
		if (error >= m_errorThreshold && count != selectedIndex) {
			set->setColor(QColor(100,32,164));
			set->setBorderColor(QColor(100,32,164));
		}
		else if (count != selectedIndex) {
			set->setColor(QColor(100,164,32));
			set->setBorderColor(QColor(100,164,32));
		}
		else {
			set->setColor(QColor(32,100,164));
			set->setBorderColor(QColor(32,100,164));
		}
		*set << error;
		series->append(set);
		chart->addSeries(series);
		series->attachAxis(axisY);
		count++;
	}
}


void ReprojectionChartView::onHoverSlot() {
	QBarSeries* series = qobject_cast<QBarSeries*>(QObject::sender());
	int index = m_barSeriesList.indexOf(series);
	update(nullptr, index);
}


void ReprojectionChartView::errorThresholdChangedSlot(double value) {
	m_errorThreshold = value;
	update();
}
