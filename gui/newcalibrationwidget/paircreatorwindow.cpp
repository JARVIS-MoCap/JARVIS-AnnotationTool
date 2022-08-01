/*****************************************************************
	* File:			  paircreatorwindow.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "paircreatorwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>


PairCreatorWindow::PairCreatorWindow(QList<QString> cameraNames, QWidget *parent) :
			QDialog(parent) {
	this->setWindowTitle("Create Camera Pair");
	this->setMinimumSize(600,250);
	QGridLayout *layout = new QGridLayout(this);
	layout->setHorizontalSpacing(20);
	QWidget *mainWidget = new QWidget(this);
	QGridLayout *mainlayout = new QGridLayout(mainWidget);
	mainlayout->setContentsMargins(0,0,0,0);
	mainlayout->setVerticalSpacing(0);
 	LabelWithToolTip *modeLabel = new LabelWithToolTip("Mode");
	twoCamButton = new QRadioButton("2 Cameras",mainWidget);
	twoCamButton->setChecked(true);
	connect(twoCamButton, &QRadioButton::toggled, this, &PairCreatorWindow::modeToggledSlot);
	threeCamButton = new QRadioButton("3 Cameras",mainWidget);
	QGroupBox *selectorBox = new QGroupBox(mainWidget);
	QGridLayout *selectorlayout = new QGridLayout(selectorBox);
	QLabel *primaryLabel = new QLabel("Primary Camera");
	primaryCombo = new QComboBox();
	intermediateLabel = new QLabel("Intermediate Camera");
	intermediateCombo = new QComboBox();
	intermediateLabel->hide();
	intermediateCombo->hide();
	QLabel *secondaryLabel = new QLabel("Secondary Camera");
	secondaryCombo = new QComboBox();
	for (const auto & cam : cameraNames) {
		primaryCombo->addItem(cam);
		intermediateCombo->addItem(cam);
		secondaryCombo->addItem(cam);
	}

	selectorlayout->addWidget(primaryLabel,0,0);
	selectorlayout->addWidget(primaryCombo,0,1);
	selectorlayout->addWidget(intermediateLabel,1,0);
	selectorlayout->addWidget(intermediateCombo,1,1);
	selectorlayout->addWidget(secondaryLabel,2,0);
	selectorlayout->addWidget(secondaryCombo,2,1);
	mainlayout->addWidget(modeLabel,0,0);
	mainlayout->addWidget(twoCamButton,0,1);
	mainlayout->addWidget(threeCamButton,0,2);
	mainlayout->addWidget(selectorBox,1,0,1,3);

	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spacer->setMaximumSize(1000,50);
	cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(40,40);
	connect(cancelButton, &QPushButton::clicked, this, &PairCreatorWindow::accept);
	addButtom = new QPushButton("Add");
	addButtom->setIcon(QIcon::fromTheme("add"));
	addButtom->setMinimumSize(40,40);
	connect(addButtom, &QPushButton::clicked, this, &PairCreatorWindow::reject);

	layout->addWidget(mainWidget,0,0,1,3);
	layout->addWidget(spacer,1,0);
	layout->addWidget(cancelButton,1,1);
	layout->addWidget(addButtom,1,2);
}


void PairCreatorWindow::modeToggledSlot(bool toggle) {
	if (toggle) {
		intermediateLabel->hide();
		intermediateCombo->hide();
		m_mode = 0;
	}
	else {
		intermediateLabel->show();
		intermediateCombo->show();
		m_mode = 1;
	}
}


void PairCreatorWindow::cancelClickedSlot() {
	this->close();
}


QList<QString> PairCreatorWindow::getCameraPair() {
	QList<QString> pair;
	QString primary = primaryCombo->currentText();
	QString intermediate = intermediateCombo->currentText();
	QString secondary = secondaryCombo->currentText();

	if (m_mode == 0) {
		if (primary != "" && secondary != "" && (primary != secondary)) {
			pair.append(primaryCombo->currentText());
			pair.append(secondaryCombo->currentText());
		}
		else {
			QErrorMessage *m_errorMsg = new QErrorMessage(this);
			m_errorMsg->showMessage("Primary and Secondary can't be the same camera!");
		}
	}
	else if (m_mode == 1) {
		if (primary != "" && intermediate != "" && secondary != "" && (primary != intermediate) && (primary != secondary) && (secondary != intermediate)) {
			pair.append(primaryCombo->currentText());
			pair.append(intermediateCombo->currentText());
			pair.append(secondaryCombo->currentText());
		}
		else {
			QErrorMessage *m_errorMsg = new QErrorMessage(this);
			m_errorMsg->showMessage("Primary, Intermediate and Secondary can't be the same camera!");
		}
	}
	return pair;
}


void PairCreatorWindow::setCameraPair(QList<QString> cameraPair) {
	if (cameraPair.size() == 2) {
		intermediateLabel->hide();
		intermediateCombo->hide();
		m_mode = 0;
		primaryCombo->setCurrentText(cameraPair[0]);
		secondaryCombo->setCurrentText(cameraPair[1]);
	}
	else {
		threeCamButton->setChecked(true);
		intermediateLabel->show();
		intermediateCombo->show();
		m_mode = 1;
		primaryCombo->setCurrentText(cameraPair[0]);
		intermediateCombo->setCurrentText(cameraPair[1]);
		secondaryCombo->setCurrentText(cameraPair[2]);
	}
}
