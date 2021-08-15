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
		explicit CompositionChartView();
		void update(QList<DatasetExportItem> *datasetExportItems = nullptr);

		signals:
			void hoverStarted(int idx);
			void hoverEnded();

	private:
		QChart *chart;
		QPieSeries *m_series;

	private slots:
		void onHoverSlot(QPieSlice *slice, bool state);

};

#endif
