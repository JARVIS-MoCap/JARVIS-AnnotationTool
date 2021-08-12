/*------------------------------------------------------------
 *  compositionchartview.hpp
 *  Created: 12. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef COMPOSITIONCHARTVIEW_H
#define COMPOSITIONCHARTVIEW_H

#include "globals.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QtCharts>


class CompositionChartView : public QChartView {
	Q_OBJECT
	public:
		explicit CompositionChartView(QList<QString> names, QList<int> frameNumbers);

	private:
		void update();
		QList<int> m_frameNumbers;
		QList <QString> m_names;
		QChart *chart;

	private slots:
		void onHoverSlot(QPieSlice *slice, bool state);

};

#endif
