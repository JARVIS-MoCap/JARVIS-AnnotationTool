/*****************************************************************
 * File:			datasetprogressinfowindow.hpp
 * Created: 	24. July 2021
 * Author:   	Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 ******************************************************************/

#ifndef DATASETPROGRSSINFOWINDOW_H
#define DATASETPROGRSSINFOWINDOW_H

#include "globals.hpp"


#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QKeyEvent>


class DatasetProgressInfoWindow : public QDialog {
	Q_OBJECT
	public:
		explicit DatasetProgressInfoWindow(QWidget *parent = nullptr);

	public slots:
		void recordingBeingProcessedChangedSlot(QString recording);
		void segmentNameChangedSlot(QString segmentName);
		void dctProgressSlot(int index, int windowSize);
		void startedClusteringSlot();
		void finishedClusteringSlot();
		void copyingFramesSlot();
		void copyImagesStatusSlot(int frameCount, int totalNumFrames);


	signals:

	private:
		QLabel *recordingLabel;
		QLabel *operationLabel;
		QProgressBar *progressBar;
		QString m_currentSegmentName = "";
		QPushButton *cancelButton;


		void keyPressEvent(QKeyEvent *e);

	private slots:


};

#endif
