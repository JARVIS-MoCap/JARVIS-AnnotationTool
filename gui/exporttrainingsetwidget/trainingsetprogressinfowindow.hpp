/*****************************************************************
 * File:			trainingsetprogressinfowindow.hpp
 * Created: 	19. February 2022
 * Author:   	Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 ******************************************************************/

#ifndef TRAININGSETPROGRESSINFOWINDOW_H
#define TRAININGSETPROGRESSINFOWINDOW_H

#include "globals.hpp"


#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QKeyEvent>
#include <QGroupBox>
#include <QGridLayout>


class TrainingSetInfoWindow : public QDialog {
	Q_OBJECT
	public:
		explicit TrainingSetInfoWindow(QWidget *parent = nullptr);

	public slots:
		void copyImagesProgressSlot(int frameCount, int totalNumFrames, const QString& setName);
		void exportFinishedSlot(bool success);

	signals:

	private:
		QGridLayout *layout;
		QGroupBox *progressGroup;
		QLabel *setLabel;
		QLabel *progressLabel;
		QProgressBar* progressBar;
		QPushButton *cancelButton;


		void keyPressEvent(QKeyEvent *e);

	private slots:


};

#endif
