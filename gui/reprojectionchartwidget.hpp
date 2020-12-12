/*------------------------------------------------------------
 *  reprojectionchartwidget.hpp
 *  Created: 03. December 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef REPROJECTIONCHARTWIDGET_H
#define REPROJECTIONCHARTWIDGET_H

#include "globals.hpp"
#include "dataset.hpp"
#include "reprojectionchartview.hpp"


#include <QGridLayout>
#include <QLabel>
#include <QtCharts>

class ReprojectionChartWidget : public QWidget {
	Q_OBJECT
	public:
		explicit ReprojectionChartWidget(QWidget *parent = nullptr);


	public slots:
		void datasetLoadedSlot();
		void reprojectionErrorsUpdatedSlot(QMap<QString, std::vector<double> *> reprojectionErrors);

	signals:
		void errorThresholdChanged(double value);

	private:
		QTabWidget *chartsTabWidget;
		QMap<QString, ReprojectionChartView*> reprojectionChartViews;

	private slots:
};




#endif
