/*****************************************************************
 * File:			progressinfowindow.hpp
 * Created: 	24. July 2021
 * Author:   	Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 ******************************************************************/

#ifndef PROGRSSINFOWINDOW_H
#define PROGRSSINFOWINDOW_H

#include "globals.hpp"


#include <QPushButton>
#include <QLabel>
#include <QProgressBar>


class ProgressInfoWindow : public QWidget {
	Q_OBJECT
	public:
		explicit ProgressInfoWindow(QWidget *parent = nullptr);

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

	private slots:


};

#endif
