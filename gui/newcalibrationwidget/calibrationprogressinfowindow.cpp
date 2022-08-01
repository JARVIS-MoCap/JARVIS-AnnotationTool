/*****************************************************************
	* File:			  calibrationprogressinfowindow.cpp
	* Created: 	  24. July 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "calibrationprogressinfowindow.hpp"

#include <QGridLayout>
#include <QGroupBox>


CalibrationProgressInfoWindow::CalibrationProgressInfoWindow(QList<QString> cameraNames,
			QList<QList<QString>> cameraPairs, QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Calibration Set Creation Progress");
	QGridLayout *layout = new QGridLayout(this);
	operationLabel = new QLabel("");
	operationLabel->setWordWrap(true);
	operationLabel->setMinimumSize(100,30);
	QGroupBox *progressBarBox = new QGroupBox("");
	QGridLayout *progresslayout = new QGridLayout(progressBarBox);
	progresslayout->setContentsMargins(0,0,0,0);
	progressStackWidget = new QStackedWidget();
	progresslayout->addWidget(progressStackWidget,0,0);
	QWidget *intrinsicsWidget = new QWidget();
	QGridLayout *intrinsicslayout = new QGridLayout(intrinsicsWidget);
	for (int i = 0; i < cameraNames.size(); i++) {
		QLabel *camLabel = new QLabel(cameraNames[i]);
		QProgressBar* bar = new QProgressBar(this);
		bar->setMinimumSize(500,25);
		bar->setMaximumSize(9999,25);
		intrinsicsProgressBars.append(bar);
		intrinsicslayout->addWidget(camLabel, i,0);
		intrinsicslayout->addWidget(bar,i,1);
	}

	QWidget *extrinsicsWidget = new QWidget();
	QGridLayout *extrinsicslayout = new QGridLayout(extrinsicsWidget);
	for (int i = 0; i < cameraPairs.size(); i++) {
		QLabel *pairLabel = new QLabel(cameraPairs[i][0] + " --> " +
																	 cameraPairs[i][cameraPairs[i].size()-1]);
 		QProgressBar* bar = new QProgressBar(this);
		bar->setMinimumSize(500, 25);
		bar->setMaximumSize(9999, 25);
		extrinsicsProgressBars.append(bar);
		extrinsicslayout->addWidget(pairLabel, i,0);
		extrinsicslayout->addWidget(bar,i,1);
	}

	progressStackWidget->addWidget(intrinsicsWidget);
	progressStackWidget->addWidget(extrinsicsWidget);
	progressStackWidget->setCurrentWidget(intrinsicsWidget);

	cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(30,30);
	connect(cancelButton, &QPushButton::clicked, this, &CalibrationProgressInfoWindow::reject);

	layout->addWidget(operationLabel,0,0);
	layout->addWidget(progressBarBox,1,0);
	layout->addWidget(cancelButton,2,0, Qt::AlignRight);
}


void CalibrationProgressInfoWindow::updateIntrinsicsProgressSlot(int count,
			int frameCount, int threadNumber) {
	progressStackWidget->widget(1)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	progressStackWidget->widget(0)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->adjustSize();
	operationLabel->setText("Calibrating Intrinsics...");
	intrinsicsProgressBars[threadNumber]->setRange(0,frameCount);
	intrinsicsProgressBars[threadNumber]->setValue(count);
}


void CalibrationProgressInfoWindow::updateExtrinsicsProgressSlot(int count,
			int frameCount, int threadNumber) {
	progressStackWidget->widget(0)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	progressStackWidget->widget(1)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	progressStackWidget->setCurrentIndex(1);
	this->adjustSize();
	operationLabel->setText("Calibrating Extrinsics...");
	extrinsicsProgressBars[threadNumber]->setRange(0,frameCount);
	extrinsicsProgressBars[threadNumber]->setValue(count);
}


void CalibrationProgressInfoWindow::keyPressEvent(QKeyEvent *e) {
	if(e->key() != Qt::Key_Escape)
		QDialog::keyPressEvent(e);
}
