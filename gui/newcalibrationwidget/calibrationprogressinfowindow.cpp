/*****************************************************************
 * File:			calibrationprogressinfowindow.cpp
 * Created: 	24. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "calibrationprogressinfowindow.hpp"

#include <QGridLayout>
#include <QGroupBox>


CalibrationProgressInfoWindow::CalibrationProgressInfoWindow(QList<QString> cameraNames, QList<QList<QString>> cameraPairs, QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	this->resize(700,40+30*cameraNames.size());
	this->setMinimumSize(500,40+30*cameraNames.size());
	setWindowTitle("Calibration Set Creation Progress");
	QGridLayout *layout = new QGridLayout(this);
	operationLabel = new QLabel("");
	operationLabel->setWordWrap(true);
	operationLabel->setMinimumSize(100,40);
	QGroupBox *progressBarBox = new QGroupBox("");
	QGridLayout *progresslayout = new QGridLayout(progressBarBox);
	progresslayout->setMargin(0);
	progressStackWidget = new QStackedWidget();
	progresslayout->addWidget(progressStackWidget,0,0);
	QWidget *intrinsicsWidget = new QWidget();
	QGridLayout *intrinsicslayout = new QGridLayout(intrinsicsWidget);
	for (int i = 0; i < cameraNames.size(); i++) {
		QLabel *camLabel = new QLabel(cameraNames[i]);
		QProgressBar* bar = new QProgressBar(this);
		intrinsicsProgressBars.append(bar);
		intrinsicslayout->addWidget(camLabel, i,0);
		intrinsicslayout->addWidget(bar,i,1);
	}

	QWidget *extrinsicsWidget = new QWidget();
	QGridLayout *extrinsicslayout = new QGridLayout(extrinsicsWidget);
	for (int i = 0; i < cameraPairs.size(); i++) {
		QLabel *pairLabel = new QLabel(cameraPairs[i][0] + " --> " + cameraPairs[i][cameraPairs[i].size()-1]);
 		QProgressBar* bar = new QProgressBar(this);
		extrinsicsProgressBars.append(bar);
		extrinsicslayout->addWidget(pairLabel, i,0);
		extrinsicslayout->addWidget(bar,i,1);
	}

	progressStackWidget->addWidget(intrinsicsWidget);
	progressStackWidget->addWidget(extrinsicsWidget);
	progressStackWidget->setCurrentWidget(intrinsicsWidget);

	layout->addWidget(operationLabel,0,0);
	layout->addWidget(progressBarBox,1,0);
}

void CalibrationProgressInfoWindow::updateIntrinsicsProgressSlot(int count, int frameCount, int threadNumber) {
	operationLabel->setText("Calibrating Intrinsics...");
	intrinsicsProgressBars[threadNumber]->setRange(0,frameCount);
	intrinsicsProgressBars[threadNumber]->setValue(count);
}

void CalibrationProgressInfoWindow::updateExtrinsicsProgressSlot(int count, int frameCount, int threadNumber) {
	progressStackWidget->setCurrentIndex(1);
	operationLabel->setText("Calibrating Extrinsics...");
	extrinsicsProgressBars[threadNumber]->setRange(0,frameCount);
	extrinsicsProgressBars[threadNumber]->setValue(count);
}
