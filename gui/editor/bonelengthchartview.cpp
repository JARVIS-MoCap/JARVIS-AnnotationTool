/*****************************************************************
	* File:			  bonelengthchartview.cpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "bonelengthchartview.hpp"


BoneLengthChartView::BoneLengthChartView() {
}


void BoneLengthChartView::update(std::vector<double> *boneLengthErrors, int selectedIndex) {
	if (boneLengthErrors != nullptr) {
		m_boneLengthErrors = boneLengthErrors;
	}
	if (m_boneLengthErrors == nullptr) {
		return;
	}
	QChart *chart = new QChart();
	chart->setTheme(QChart::ChartThemeDark);
	chart->setBackgroundBrush(QBrush(QColor(34, 36, 40)));
	chart->legend()->setVisible(false);
	if (selectedIndex != -1) {
		chart->setTitle(Dataset::dataset->skeleton()[selectedIndex].name);
	}
	else {
		chart->setTitle(" ");
	}
	this->setChart(chart);
	if (m_boneLengthErrors->size() != 0) {
		float maxValue = *std::max_element(m_boneLengthErrors->begin(), m_boneLengthErrors->end());
		float minValue = *std::min_element(m_boneLengthErrors->begin(), m_boneLengthErrors->end());
		float range = std::max(-minValue, maxValue);
		QValueAxis *axisY = new QValueAxis();
		axisY->setRange(0,range);
		chart->addAxis(axisY, Qt::AlignLeft);

		int count = 0;
		m_barSeriesList.clear();
		for (const auto& error : *m_boneLengthErrors) {
			double error_abs = std::abs(error);
			QBarSeries *series = new QBarSeries();
			m_barSeriesList.append(series);
			connect(series, &QBarSeries::hovered, this, &BoneLengthChartView::onHoverSlot);
			QBarSet * set = new QBarSet("Test");
			if (error_abs >= m_errorThreshold && count != selectedIndex) {
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
			*set << error_abs;
			series->append(set);
			chart->addSeries(series);
			series->attachAxis(axisY);
			count++;
		}
	}
}


void BoneLengthChartView::onHoverSlot() {
	QBarSeries* series = qobject_cast<QBarSeries*>(QObject::sender());
	int index = m_barSeriesList.indexOf(series);
	update(nullptr, index);
}


void BoneLengthChartView::boneLengthErrorThresholdChangedSlot(double value) {
	m_errorThreshold = value;
	update();
}
