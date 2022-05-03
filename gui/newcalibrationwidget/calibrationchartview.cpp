/*****************************************************************
	* File:			  datasetcontrolview.cpp
	* Created: 	  03. December 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "calibrationchartview.hpp"

#include <random>


CalibrationChartView::CalibrationChartView(QList<QString> names,
			std::vector<double> reproErrors, QString mode) :
			m_names(names), m_reproErrors(reproErrors), m_mode(mode) {
	if (m_mode == "interactive") {
		update(0);
	}
	else {
		update();
	}
}


void CalibrationChartView::update(int selectedIndex) {
	QChart *chart = new QChart();
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));
	chart->legend()->setVisible(false);
	if (selectedIndex != -1) {
		if (m_mode == "interactive") {
			chart->setTitle(m_names[selectedIndex]);
		}
	}
	else {
		if (m_mode == "interactive") {
			chart->setTitle(" ");
		}
	}
	float maxValue = *std::max_element(m_reproErrors.begin(), m_reproErrors.end());

	QValueAxis *axisY = new QValueAxis();
	axisY->setTitleText("Reprojection Error [px]");
	axisY->setRange(0,maxValue);

	if (m_mode != "interactive") {
		QBarCategoryAxis *axisX = new QBarCategoryAxis();
		axisX->append(m_names);
		chart->addAxis(axisX, Qt::AlignLeft);
	}

	chart->addAxis(axisY, Qt::AlignBottom);
	this->setChart(chart);
	int count = 0;
	m_barSeriesList.clear();
	for (const auto& error : m_reproErrors) {
		QHorizontalStackedBarSeries *series = new QHorizontalStackedBarSeries();
		m_barSeriesList.append(series);
		if (m_mode == "interactive") {
			connect(series, &QBarSeries::hovered, this, &CalibrationChartView::onHoverSlot);
		}
		QBarSet * set = new QBarSet("Test");
		if (count != selectedIndex) {
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


void CalibrationChartView::onHoverSlot() {
	QHorizontalStackedBarSeries* series = qobject_cast<QHorizontalStackedBarSeries*>(QObject::sender());
	int index = m_barSeriesList.indexOf(series);
	update(index);
}
