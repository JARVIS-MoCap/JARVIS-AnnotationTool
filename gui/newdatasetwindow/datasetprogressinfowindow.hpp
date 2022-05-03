/*****************************************************************
 * File:				datasetprogressinfowindow.hpp
 * Created: 		24. July 2021
 * Author:		 	Timo Hueser
 * Contact: 	 	timo.hueser@gmail.com
 * Copyright:  	2022 Timo Hueser
 * License:    	LGPL v2.1
 ******************************************************************/

#ifndef DATASETPROGRSSINFOWINDOW_H
#define DATASETPROGRSSINFOWINDOW_H

#include "globals.hpp"


#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QKeyEvent>
#include <QGroupBox>
#include <QGridLayout>


class DatasetProgressInfoWindow : public QDialog {
	Q_OBJECT
	public:
		explicit DatasetProgressInfoWindow(QWidget *parent = nullptr);

	public slots:
		void recordingBeingProcessedChangedSlot(QString recording, QList<QString> cameras);
		void segmentNameChangedSlot(QString segmentName);
		void dctProgressSlot(int index, int windowSize, int threadNumber);
		void startedClusteringSlot();
		void finishedClusteringSlot();
		void copyingFramesSlot();
		void copyImagesStatusSlot(int frameCount, int totalNumFrames, int threadNumber);


	signals:

	private:
		QGridLayout *layout;
		QGroupBox *progressGroup;
		QLabel *recordingLabel;
		QLabel *operationLabel;
		QList<QProgressBar*> progressBars;
		QString m_currentSegmentName = "";
		QPushButton *cancelButton;


		void keyPressEvent(QKeyEvent *e);

	private slots:


};

#endif
