/*****************************************************************
 * File:			progressinfowindow.cpp
 * Created: 	24. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "datasetprogressinfowindow.hpp"


DatasetProgressInfoWindow::DatasetProgressInfoWindow(QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Dataset Creation Progress");
	layout = new QGridLayout(this);
	recordingLabel = new QLabel("");
	recordingLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	recordingLabel->setWordWrap(true);
	operationLabel = new QLabel("");
	operationLabel->setWordWrap(true);

	progressGroup = new QGroupBox(this);


	cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(30,30);
	connect(cancelButton, &QPushButton::clicked, this, &DatasetProgressInfoWindow::reject);

	layout->addWidget(recordingLabel,0,0);
	layout->addWidget(operationLabel,1,0);
	layout->addWidget(progressGroup,2,0);
	layout->addWidget(cancelButton,3,0, Qt::AlignRight);


}


void DatasetProgressInfoWindow::recordingBeingProcessedChangedSlot(QString recording, QList<QString> cameras) {
	std::cout << "recordingBeingProcessedChangedSlot " << cameras.size() << std::endl;
	delete progressGroup;
	progressGroup = new QGroupBox(this);
	QGridLayout *progresslayout = new QGridLayout(progressGroup);
	progressBars.clear();
	recordingLabel->setText("Processing Recording " + recording + ":");
	for (int i = 0; i < cameras.size(); i++) {
		QLabel *camLabel = new QLabel(cameras[i]);
		QProgressBar* bar = new QProgressBar(this);
		bar->setMinimumSize(500,25);
		bar->setMaximumSize(9999,25);
		progressBars.append(bar);
		progresslayout->addWidget(camLabel, i,0);
		progresslayout->addWidget(bar,i,1);
		layout->addWidget(progressGroup,2,0);
	}
}

void DatasetProgressInfoWindow::segmentNameChangedSlot(QString segmentName) {
	m_currentSegmentName = segmentName;
}

void DatasetProgressInfoWindow::dctProgressSlot(int index, int windowSize, int threadNumber) {
	operationLabel->setText("Extracting image features for k-means Clustering for segement \"" + m_currentSegmentName + "\"...");
	if (threadNumber < progressBars.size()) {
		progressBars[threadNumber]->setRange(0, windowSize);
		progressBars[threadNumber]->setValue(index);
	}
}

void DatasetProgressInfoWindow::startedClusteringSlot() {
	operationLabel->setText("Started k-means Clustering. This might take a while...");
	progressGroup->hide();
}

void DatasetProgressInfoWindow::finishedClusteringSlot() {
	operationLabel->setText("Finished k-means Clustering!");
	progressGroup->show();
}

void DatasetProgressInfoWindow::copyingFramesSlot() {
}

void DatasetProgressInfoWindow::copyImagesStatusSlot(int frameCount, int totalNumFrames, int threadNumber) {
	if (m_currentSegmentName == "") {
		operationLabel->setText("Copying Frames to dataset folder...");
	}
	else {
		operationLabel->setText("Copying Frames for segment \"" + m_currentSegmentName + "\" to dataset folder...");
	}
	if (threadNumber < progressBars.size()) {
		progressBars[threadNumber]->setRange(0, totalNumFrames);
		progressBars[threadNumber]->setValue(frameCount);
	}
}

void DatasetProgressInfoWindow::keyPressEvent(QKeyEvent *e) {
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}
