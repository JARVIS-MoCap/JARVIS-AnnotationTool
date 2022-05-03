/*****************************************************************
	* File:			  reprojectionchartwidget.cpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "reprojectionchartwidget.hpp"

#include <random>


ReprojectionChartWidget::ReprojectionChartWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);

	chartsTabWidget = new QTabWidget(this);
	chartsTabWidget->setStyleSheet("QTabBar::tab{padding:4px 6px;"
																		"background-color:palette(base);"
																		"border-bottom:3px solid palette(alternate-base);}"
																	"QTabBar::tab:selected,QTabBar::tab:hover{"
																	"background-color: palette(alternate-base);}");

	layout->addWidget(chartsTabWidget,0,0);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals

	//<-> Relayed Signals
}


void ReprojectionChartWidget::datasetLoadedSlot() {
	chartsTabWidget->clear();
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		ReprojectionChartView * chart = new ReprojectionChartView();
		connect(this, &ReprojectionChartWidget::errorThresholdChanged, chart, &ReprojectionChartView::errorThresholdChangedSlot);
		reprojectionChartViews[entity] = chart;
		chart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
		std::default_random_engine generator;
		std::normal_distribution<double> distribution(10.0,2.0);
		std::vector<double> *reproErrors = new std::vector<double>;
		for (int i = 0; i < 26; i++) {
			reproErrors->push_back(distribution(generator));
		}
		chart->update(reproErrors);
		chartsTabWidget->addTab(chart, entity);
	}
}


void ReprojectionChartWidget::reprojectionErrorsUpdatedSlot(QMap<QString, std::vector<double> *> reprojectionErrors) {
	for (const auto& entity : reprojectionErrors.keys()) {
		reprojectionChartViews[entity]->update(reprojectionErrors[entity]);
	}
}
