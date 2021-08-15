/*****************************************************************
	* File:			  datasetconrolwidget.hpp
	* Created: 	  03. December 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

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

	signals:
		void frameSelectionChanged(int index);
		void imgSetChanged(int index);

	public slots:
		void datasetLoadedSlot();
		void frameChangedSlot(int imgSetIndex, int frameIndex);
		void keypointStateChangedSlot(KeypointState state,
					KeypointState previousSate, int frameIndex);

	private:
		void getAnnotationCounts(int frameIndex, int &annotatedCount,
					int &totalCount);

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
