/*****************************************************************
	* File:			  calibrationprogressinfowindow.hpp
	* Created: 	  24. July 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#ifndef CALIBRATIONPROGRESSINFOWINDOW_H
#define CALIBRATIONPROGRESSINFOWINDOW_H

#include "globals.hpp"

#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QStackedWidget>
#include <QKeyEvent>


class CalibrationProgressInfoWindow : public QDialog {
	Q_OBJECT

	public:
		explicit CalibrationProgressInfoWindow(QList<QString> cameraNames, QList<QList<QString>> cameraPairs, QWidget *parent = nullptr);

	public slots:
		void updateIntrinsicsProgressSlot(int count, int frameCount, int threadNumber);
		void updateExtrinsicsProgressSlot(int count, int frameCount, int threadNumber);

	private:
		QLabel *operationLabel;
		QStackedWidget *progressStackWidget;
		QList<QProgressBar*> intrinsicsProgressBars;
		QList<QProgressBar*> extrinsicsProgressBars;
		QPushButton *cancelButton;

		void keyPressEvent(QKeyEvent *e);

	private slots:
};

#endif
