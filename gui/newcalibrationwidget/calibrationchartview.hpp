/*****************************************************************
	* File:			  datasetcontrolview.hpp
	* Created: 	  03. December 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef CALIBRATIONCHARTVIEW_H
#define CALIBRATIONCHARTVIEW_H

#include "globals.hpp"
#include "dataset.hpp"


#include <QGridLayout>
#include <QLabel>
#include <QtCharts>


class CalibrationChartView : public QChartView {
	Q_OBJECT

	public:
		explicit CalibrationChartView(QList<QString> names, std::vector<double> reproErrors, QString mode = "normal");

	private:
		void update(int selectedIndex = -1);
		QList <QString> m_names;
		std::vector<double> m_reproErrors;
		QString m_mode;
		QList<QHorizontalStackedBarSeries*> m_barSeriesList;

	private slots:
		void onHoverSlot();
};

#endif
