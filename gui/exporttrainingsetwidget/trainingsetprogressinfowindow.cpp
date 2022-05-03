/*****************************************************************
 * File:			trainingsetprogressinfowindow.cpp
 * Created: 	19. February 2022
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "trainingsetprogressinfowindow.hpp"


TrainingSetInfoWindow::TrainingSetInfoWindow(QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Trainingset Creation Progress");
	layout = new QGridLayout(this);
	setLabel = new QLabel("");
	setLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));

	QGroupBox *progressGroup = new QGroupBox(this);
	QGridLayout *progresslayout = new QGridLayout(progressGroup);

	progressLabel = new QLabel("Copying Frames...");
	progressLabel->setWordWrap(true);
	progressBar = new QProgressBar(this);
	progresslayout->addWidget(progressLabel,0,0);
	progresslayout->addWidget(progressBar,1,0);

	cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(30,30);
	connect(cancelButton, &QPushButton::clicked, this, &TrainingSetInfoWindow::reject);

	layout->addWidget(setLabel,0,0);
	layout->addWidget(progressGroup,1,0);
	layout->addWidget(cancelButton,2,0, Qt::AlignRight);
}


void TrainingSetInfoWindow::copyImagesProgressSlot(int frameCount, int totalNumFrames, const QString& setName) {
	setLabel->setText(setName);
	progressBar->setRange(0, totalNumFrames);
	progressBar->setValue(frameCount);
}

void TrainingSetInfoWindow::keyPressEvent(QKeyEvent *e) {
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}


void TrainingSetInfoWindow::exportFinishedSlot() {
	accept();
}
