/*****************************************************************
	* File:			  datasetcontrolview.hpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef REPROJECTIONCHARTVIEW_H
#define REPROJECTIONCHARTVIEW_H

#include "globals.hpp"
#include "dataset.hpp"


#include <QGridLayout>
#include <QLabel>
#include <QtCharts>


class ReprojectionChartView : public QChartView {
	Q_OBJECT
	public:
		explicit ReprojectionChartView();

	public slots:
		void update(std::vector<double> *reproErrors = nullptr, int selectedIndex = -1);
		void errorThresholdChangedSlot(double value);

	private:
		double m_errorThreshold = 10.0;
		std::vector<double> *m_reproErrors;
		QList<QBarSeries*> m_barSeriesList;

	private slots:
		void onHoverSlot();
};

#endif
