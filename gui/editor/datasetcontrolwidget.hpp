/*****************************************************************
	* File:			  datasetconrolwidget.hpp
	* Created: 	  03. December 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
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
#include <QComboBox>


class DatasetControlWidget : public QWidget {
	Q_OBJECT

	public:
		explicit DatasetControlWidget(QWidget *parent = nullptr);

	signals:
		void frameSelectionChanged(int index);
		void imgSetChanged(int index);
		void datasetLoaded();

	public slots:
		void datasetLoadedSlot();
		void frameChangedSlot(int imgSetIndex, int frameIndex);
		void keypointStateChangedSlot(KeypointState state,
					KeypointState previousSate, int frameIndex);

	private:
		void getAnnotationCounts(int frameIndex, int &annotatedCount,
					int &totalCount);

		QComboBox *segmentCombo;
		QLineEdit *frameSetEdit;
		QLabel *totalFrameSetLabel;
		QTableWidget *framesTable;

		int m_currentFrameIndex = 0;
		int m_currentImgSetIndex = 0;
		int *m_annotatedCounts;
		int m_totalCount;
		QString m_currentSegment;

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
		void selectionChangedSlot(int row,int);
		void imgSetChangedSlot();
		void segmentChangedSlot(const QString &segment);
};

#endif
