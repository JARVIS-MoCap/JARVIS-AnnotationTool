/*------------------------------------------------------------
 *  datasetcontrolview.cpp
 *  Created:  03. December 2020
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "calibrationchartview.hpp"

#include <random>

CalibrationChartView::CalibrationChartView(QList<QString> names, std::vector<double> reproErrors) : m_names(names), m_reproErrors(reproErrors) {
	update();
}

void CalibrationChartView::update(int selectedIndex) {
	QChart *chart = new QChart();
	//chart->setAnimationOptions(QChart::SeriesAnimations);
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));
	chart->legend()->setVisible(false);
	if (selectedIndex != -1) {
		chart->setTitle(m_names[selectedIndex]);
	}
	else {
		chart->setTitle(" ");
	}
	//float minValue = *std::min_element(m_reproErrors->begin(), m_reproErrors->end());
	float maxValue = *std::max_element(m_reproErrors.begin(), m_reproErrors.end());

	QValueAxis *axisY = new QValueAxis();
	axisY->setRange(0,maxValue);

	chart->addAxis(axisY, Qt::AlignLeft);
	this->setChart(chart);
	int count = 0;
	m_barSeriesList.clear();
	for (const auto& error : m_reproErrors) {
		QBarSeries *series = new QBarSeries();
		m_barSeriesList.append(series);
		connect(series, &QBarSeries::hovered, this, &CalibrationChartView::onHoverSlot);
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
	QBarSeries* series = qobject_cast<QBarSeries*>(QObject::sender());
	int index = m_barSeriesList.indexOf(series);
	update(index);
}
