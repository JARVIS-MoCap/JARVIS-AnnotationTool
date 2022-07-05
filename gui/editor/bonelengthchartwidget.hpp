/*****************************************************************
	* File:			  bonelengthchartwidget.hpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/


#ifndef BONELENGTHCHARTWIDGET_H
#define BONELENGTHCHARTWIDGET_H

#include "globals.hpp"
#include "dataset.hpp"
#include "bonelengthchartview.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QtCharts>


class BoneLengthChartWidget : public QWidget {
	Q_OBJECT

	public:
		explicit BoneLengthChartWidget(QWidget *parent = nullptr);

	signals:
		void boneLengthErrorThresholdChanged(double value);

	public slots:
		void datasetLoadedSlot();
		void boneLengthErrorsUpdatedSlot(QMap<QString, std::vector<double>*> boneLengthErrors);

	private:
		QTabWidget *chartsTabWidget;
		QMap<QString, BoneLengthChartView*> boneLengthChartViews;

		bool eventFilter(QObject *target, QEvent *event)
		{
			if (event->type() == QKeyEvent::KeyPress)
			{
				event->ignore();
				return true;
			}
			return QObject::eventFilter(target, event);
		}

	private slots:
};




#endif
