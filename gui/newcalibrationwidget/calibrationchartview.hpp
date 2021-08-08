/*------------------------------------------------------------
 *  calibrationchartview.hpp
 *  Created: 03. December 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

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

	public slots:

	private:
		void update(int selectedIndex = -1);
		QString m_mode;
		std::vector<double> m_reproErrors;
		QList <QString> m_names;
		QList<QHorizontalStackedBarSeries*> m_barSeriesList;

	private slots:
		void onHoverSlot();
};

#endif
