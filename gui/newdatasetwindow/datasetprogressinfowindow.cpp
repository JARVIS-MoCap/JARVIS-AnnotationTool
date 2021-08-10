/*****************************************************************
 * File:			progressinfowindow.cpp
 * Created: 	24. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "datasetprogressinfowindow.hpp"

#include <QGridLayout>


DatasetProgressInfoWindow::DatasetProgressInfoWindow(QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	this->resize(700,120);
	this->setMinimumSize(500,140);
	setWindowTitle("Dataset Creation Progress");
	QGridLayout *layout = new QGridLayout(this);
	recordingLabel = new QLabel("");
	recordingLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	recordingLabel->setWordWrap(true);
	operationLabel = new QLabel("");
	operationLabel->setWordWrap(true);
	progressBar = new QProgressBar(this);

	cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(30,30);
	connect(cancelButton, &QPushButton::clicked, this, &DatasetProgressInfoWindow::reject);

	layout->addWidget(recordingLabel,0,0);
	layout->addWidget(operationLabel,1,0);
	layout->addWidget(progressBar,3,0);
	layout->addWidget(cancelButton,4,0, Qt::AlignRight);


}


void DatasetProgressInfoWindow::recordingBeingProcessedChangedSlot(QString recording) {
	recordingLabel->setText("Processing Recording " + recording + ":");
}

void DatasetProgressInfoWindow::segmentNameChangedSlot(QString segmentName) {
	m_currentSegmentName = segmentName;
}

void DatasetProgressInfoWindow::dctProgressSlot(int index, int windowSize) {
	operationLabel->setText("Extracting image features for k-means Clustering for segement \"" + m_currentSegmentName + "\"...");
	progressBar->setRange(0, windowSize);
	progressBar->setValue(index);
}

void DatasetProgressInfoWindow::startedClusteringSlot() {
	operationLabel->setText("Started k-means Clustering. This might take a while...");
	progressBar->hide();
}

void DatasetProgressInfoWindow::finishedClusteringSlot() {
	operationLabel->setText("Finished k-means Clustering!");
	progressBar->show();
}

void DatasetProgressInfoWindow::copyingFramesSlot() {
}

void DatasetProgressInfoWindow::copyImagesStatusSlot(int frameCount, int totalNumFrames) {
	if (m_currentSegmentName == "") {
		operationLabel->setText("Copying Frames to dataset folder...");
	}
	else {
		operationLabel->setText("Copying Frames for segment \"" + m_currentSegmentName + "\" to dataset folder...");
	}
	progressBar->setRange(0, totalNumFrames);
	progressBar->setValue(frameCount);
}

void DatasetProgressInfoWindow::keyPressEvent(QKeyEvent *e) {
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}
