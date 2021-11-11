/*****************************************************************
	* File:			  bonelengthchartwidget.cpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "bonelengthchartwidget.hpp"

#include <random>


BoneLengthChartWidget::BoneLengthChartWidget(QWidget *parent) : QWidget(parent) {
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


void BoneLengthChartWidget::datasetLoadedSlot() {
	chartsTabWidget->clear();
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		BoneLengthChartView * chart = new BoneLengthChartView();
		connect(this, &BoneLengthChartWidget::boneLengthErrorThresholdChanged, chart, &BoneLengthChartView::boneLengthErrorThresholdChangedSlot);
		boneLengthChartViews[entity] = chart;
		chart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
		chartsTabWidget->addTab(chart, entity);
	}
}


void BoneLengthChartWidget::boneLengthErrorsUpdatedSlot(QMap<QString, std::vector<double>*> boneLengthErrors) {
	for (const auto& entity : boneLengthErrors.keys()) {
		std::cout << entity.toStdString() << std::endl;
	boneLengthChartViews[entity]->update(boneLengthErrors[entity]);
	}
}
