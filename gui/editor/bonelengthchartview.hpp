/*****************************************************************
	* File:			  reprojectionchartview.hpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#ifndef BONELENGTHCHARTVIEW_H
#define BONELENGTHCHARTVIEW_H

#include "globals.hpp"
#include "dataset.hpp"


#include <QGridLayout>
#include <QLabel>
#include <QtCharts>


class BoneLengthChartView : public QChartView {
	Q_OBJECT

	public:
		explicit BoneLengthChartView();

	public slots:
		void update(std::vector<double> *boneLengthErrors = nullptr, int selectedIndex = -1);
		void boneLengthErrorThresholdChangedSlot(double value);

	private:
		double m_errorThreshold = 10.0;
		std::vector<double> *m_boneLengthErrors;
		QList<QBarSeries*> m_barSeriesList;

		QChart *chart;
		QValueAxis *axisY;

	private slots:
		void onHoverSlot();
};

#endif
