/*****************************************************************
 * File:			exporttrainingsetwidget.cpp
 * Created: 	30. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "exporttrainingsetwidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QDirIterator>
#include <QThread>


ExportTrainingsetWidget::ExportTrainingsetWidget(QWidget *parent) : QWidget(parent) {
	this->setMinimumSize(600,600);
	settings = new QSettings();
	QGridLayout *layout = new QGridLayout(this);
	layout->setVerticalSpacing(20);
	layout->setHorizontalSpacing(100);
	m_errorMsg = new QErrorMessage();

	loadPresetsWindow = new PresetsWindow(&presets, "load", "Export Dataset Widget/");
	savePresetsWindow = new PresetsWindow(&presets, "save", "Export Dataset Widget/");
	connect(loadPresetsWindow, SIGNAL(loadPreset(QString)), this, SLOT(loadPresetSlot(QString)));
	connect(savePresetsWindow, SIGNAL(savePreset(QString)), this, SLOT(savePresetSlot(QString)));

	QLabel *exportTrainingsSetLabel = new QLabel("Export Trainingset");
	exportTrainingsSetLabel->setFont(QFont("Sans Serif", 18, QFont::Bold));
	exportTrainingsSetLabel->setMaximumSize(9999,50);

	QGroupBox *configurationBox = new QGroupBox("Configuration", this);
	QGridLayout *configurationlayout = new QGridLayout(configurationBox);

	QGroupBox *datasetListBox = new QGroupBox("Datasets", this);
	QGridLayout *datasetlistlayout = new QGridLayout(datasetListBox);

	QGroupBox *keypointListBox = new QGroupBox("Keypoints", this);
	QGridLayout *keypointlistlayout = new QGridLayout(keypointListBox);


	QWidget *buttonBarWidget = new QWidget(this);
	buttonBarWidget->setMaximumSize(100000,50);
	QGridLayout *buttonbarlayout = new QGridLayout(buttonBarWidget);
	saveButton = new QPushButton("Save Preset");
	saveButton->setIcon(QIcon::fromTheme("upload"));
	saveButton->setMinimumSize(40,40);
	connect(saveButton, &QPushButton::clicked, this, &ExportTrainingsetWidget::savePresetsClickedSlot);
	loadButton = new QPushButton("Load Preset");
	loadButton->setIcon(QIcon::fromTheme("download"));
	loadButton->setMinimumSize(40,40);
	connect(loadButton, &QPushButton::clicked, this, &ExportTrainingsetWidget::loadPresetsClickedSlot);
	QWidget *middleSpacer = new QWidget();
	middleSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	exportButton = new QPushButton("Export");
	exportButton->setIcon(QIcon::fromTheme("start"));
	exportButton->setMinimumSize(40,40);
	//connect(exportButton, &QPushButton::clicked, this, &ExportTrainingsetWidget::exportClickedSlot);
	buttonbarlayout->addWidget(saveButton, 0,0);
	buttonbarlayout->addWidget(loadButton,0,1);
	buttonbarlayout->addWidget(middleSpacer,0,2);
	buttonbarlayout->addWidget(exportButton,0,3);

	layout->addWidget(exportTrainingsSetLabel,0,0,1,2);
	layout->addWidget(configurationBox,1,0,2,1);
	layout->addWidget(datasetListBox,1,1);
	layout->addWidget(keypointListBox,2,1);
	layout->addWidget(buttonBarWidget, 3,0,1,2);

}




void ExportTrainingsetWidget::savePresetsClickedSlot() {
	savePresetsWindow->updateListSlot();
	savePresetsWindow->show();
}


void ExportTrainingsetWidget::loadPresetsClickedSlot() {
	loadPresetsWindow->updateListSlot();
	loadPresetsWindow->show();
}


void ExportTrainingsetWidget::savePresetSlot(const QString& preset) {
	settings->beginGroup(preset);
	settings->endGroup();
}


void ExportTrainingsetWidget::loadPresetSlot(const QString& preset) {
	settings->beginGroup(preset);
	settings->endGroup();
}
