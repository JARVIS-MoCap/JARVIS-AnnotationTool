/*------------------------------------------------------------
 *  datasetcontrolwidget.hpp
 *  Created: 03. December 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef DATASETCONTROLWIDGET_H
#define DATASETCONTROLWIDGET_H

#include "globals.hpp"
#include "dataset.hpp"
#include "switch.hpp"
#include "dataset.hpp"


#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QLineEdit>


class DatasetControlWidget : public QWidget {
	Q_OBJECT
	public:
		explicit DatasetControlWidget(QWidget *parent = nullptr);


	public slots:
		void datasetLoadedSlot();
		void frameChangedSlot(int imgSetIndex, int frameIndex);
		void keypointStateChangedSlot(KeypointState state, KeypointState previousSate, int frameIndex);

	signals:
		void frameSelectionChanged(int index);
		void imgSetChanged(int index);

	private:
		void getAnnotationCounts(int frameIndex, int &annotatedCount, int &totalCount);

		QLineEdit *imgSetEdit;
		QLabel *imgSetTotalLabel;
		QTableWidget *framesTable;

		int m_currentFrameIndex = 0;
		int m_currentImgSetIndex = 0;
		int *m_annotatedCounts;
		int m_totalCount;

	private slots:
		void selectionChangedSlot(int row,int);
		void imgSetChangedSlot();

};

#endif
