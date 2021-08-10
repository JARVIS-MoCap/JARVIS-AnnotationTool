/*****************************************************************
 * File:			newdatasetwindow.cpp
 * Created: 	07. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "newdatasetwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QThread>
#include <QMessageBox>

NewDatasetWindow::NewDatasetWindow(QWidget *parent) : QWidget(parent, Qt::Window) {
	this->resize(800,800);
	this->setMinimumSize(600,600);
	settings = new QSettings();
	setWindowTitle("New Dataset");
	QGridLayout *layout = new QGridLayout(this);

	m_datasetConfig = new DatasetConfig;
	datasetCreator = new DatasetCreator(m_datasetConfig);
	QThread *thread = new QThread;
	datasetCreator->moveToThread(thread);
	thread->start();
	datasetProgressInfoWindow = new DatasetProgressInfoWindow(this);
	m_errorMsg = new QErrorMessage();

	QLabel *newDatasetLabel = new QLabel("New Dataset");
	newDatasetLabel->setFont(QFont("Sans Serif", 18, QFont::Bold));


	loadPresetsWindow = new PresetsWindow(&presets, "load", "New Dataset Window/");
	savePresetsWindow = new PresetsWindow(&presets, "save", "New Dataset Window/");
	connect(loadPresetsWindow, SIGNAL(loadPreset(QString)), this, SLOT(loadPresetSlot(QString)));
	connect(savePresetsWindow, SIGNAL(savePreset(QString)), this, SLOT(savePresetSlot(QString)));

	QGroupBox *configBox = new QGroupBox("Configuration");
	configBox->setMinimumSize(0,180);
	QGridLayout *configlayout = new QGridLayout(configBox);
	LabelWithToolTip *datasetNameLabel = new LabelWithToolTip("New Dataset Name", "");
	datasetNameEdit = new QLineEdit(m_datasetConfig->datasetName, configBox);

	LabelWithToolTip *datasetPathLabel = new LabelWithToolTip("New Dataset Path", "");
	datasetPathWidget = new DirPathWidget("Select new Dataset Path");
	LabelWithToolTip *frameSetsRecordingLabel = new LabelWithToolTip("Framesets to extract per Recording", "kdsff");
	frameSetsRecordingBox = new QSpinBox(configBox);
	frameSetsRecordingBox->setMinimum(0);
	frameSetsRecordingBox->setMaximum(9999999);
	frameSetsRecordingBox->setValue(m_datasetConfig->frameSetsRecording);
	LabelWithToolTip *samplingMethodLabel = new LabelWithToolTip("Sampling Method", "kdsff");
	samplingMethodCombo = new QComboBox(configBox);
	samplingMethodCombo->addItem("uniform");
	samplingMethodCombo->addItem("kmeans");
	samplingMethodCombo->setCurrentText(m_datasetConfig->samplingMethod);

	QGroupBox *recordingsBox = new QGroupBox("Recordings");
	QGridLayout *recordingslayout = new QGridLayout(recordingsBox);
	recordingsTable = new RecordingsTable("Recordings", m_datasetConfig);
	recordingslayout->addWidget(recordingsTable,0,0);
	recordingslayout->setMargin(0);

	QGroupBox *entitiesBox = new QGroupBox("Entities");
	QGridLayout *entitieslayout = new QGridLayout(entitiesBox);
	entitiesItemList = new ConfigurableItemList("Entities");
	entitieslayout->addWidget(entitiesItemList,0,0);
	entitieslayout->setMargin(0);

	QGroupBox *keypointsBox = new QGroupBox("Keypoints");
	QGridLayout *keypointslayout = new QGridLayout(keypointsBox);
	keypointsItemList = new ConfigurableItemList("Keypoints");
	keypointslayout->addWidget(keypointsItemList,0,0);
	keypointslayout->setMargin(0);

	QWidget *buttonBarWidget = new QWidget(this);
	buttonBarWidget->setMaximumSize(100000,50);
	QGridLayout *buttonbarlayout = new QGridLayout(buttonBarWidget);
	saveButton = new QPushButton("Save Preset");
	saveButton->setIcon(QIcon::fromTheme("upload"));
	saveButton->setMinimumSize(40,40);
	connect(saveButton, &QPushButton::clicked, this, &NewDatasetWindow::savePresetsClickedSlot);
	loadButton = new QPushButton("Load Preset");
	loadButton->setIcon(QIcon::fromTheme("download"));
	loadButton->setMinimumSize(40,40);
	connect(loadButton, &QPushButton::clicked, this, &NewDatasetWindow::loadPresetsClickedSlot);
	QWidget *middleSpacer = new QWidget();
	middleSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	createButton = new QPushButton("Create");
	createButton->setMinimumSize(40,40);
	connect(createButton, &QPushButton::clicked, this, &NewDatasetWindow::createDatasetClickedSlot);
	buttonbarlayout->addWidget(saveButton, 0,0);
	buttonbarlayout->addWidget(loadButton,0,1);
	buttonbarlayout->addWidget(middleSpacer,0,2);
	buttonbarlayout->addWidget(createButton,0,3);

	configlayout->addWidget(datasetNameLabel,0,0);
	configlayout->addWidget(datasetNameEdit,0,1,1,2);
	configlayout->addWidget(datasetPathLabel,1,0);
	configlayout->addWidget(datasetPathWidget,1,1,1,2);
	configlayout->addWidget(frameSetsRecordingLabel,2,0);
	configlayout->addWidget(frameSetsRecordingBox,2,1,1,2);
	configlayout->addWidget(samplingMethodLabel,3,0);
	configlayout->addWidget(samplingMethodCombo,3,1,1,2);

	layout->addWidget(newDatasetLabel,0,0,1,2);
	layout->addWidget(configBox,1,0,1,2);
	layout->addWidget(recordingsBox,2,0,1,2);
	layout->addWidget(entitiesBox,3,0);
	layout->addWidget(keypointsBox,3,1);
	layout->addWidget(buttonBarWidget,4,0,1,2);

	connect(this, &NewDatasetWindow::createDataset, datasetCreator, &DatasetCreator::createDatasetSlot);
	connect(datasetCreator, &DatasetCreator::datasetCreated, this, &NewDatasetWindow::datasetCreatedSot);
	connect(datasetCreator, &DatasetCreator::datasetCreationFailed, this, &NewDatasetWindow::datasetCreationFailedSlot);

	//Signal relay
	connect(datasetCreator, &DatasetCreator::dctProgress, datasetProgressInfoWindow, &DatasetProgressInfoWindow::dctProgressSlot);
	connect(datasetCreator, &DatasetCreator::recordingBeingProcessedChanged, datasetProgressInfoWindow, &DatasetProgressInfoWindow::recordingBeingProcessedChangedSlot);
	connect(datasetCreator, &DatasetCreator::currentSegmentChanged, datasetProgressInfoWindow, &DatasetProgressInfoWindow::segmentNameChangedSlot);
	connect(datasetCreator, &DatasetCreator::startedClustering, datasetProgressInfoWindow, &DatasetProgressInfoWindow::startedClusteringSlot);
	connect(datasetCreator, &DatasetCreator::finishedClustering, datasetProgressInfoWindow, &DatasetProgressInfoWindow::finishedClusteringSlot);
	connect(datasetCreator, &DatasetCreator::copyImagesStatus, datasetProgressInfoWindow, &DatasetProgressInfoWindow::copyImagesStatusSlot);
	connect(datasetProgressInfoWindow, &DatasetProgressInfoWindow::rejected, datasetCreator, &DatasetCreator::cancelCreationSlot);



}

void NewDatasetWindow::createDatasetClickedSlot() {
	m_datasetConfig->datasetName = datasetNameEdit->text();
	m_datasetConfig->datasetPath = datasetPathWidget->path();
	m_datasetConfig->frameSetsRecording = frameSetsRecordingBox->value();
	m_datasetConfig->samplingMethod = samplingMethodCombo->currentText();

	if (m_datasetConfig->datasetPath == "") {
		m_errorMsg->showMessage("Dataset Path is empty. Dataset Creation aborted...");
		return;
	}
	if (m_datasetConfig->datasetName == "") {
		m_errorMsg->showMessage("Dataset Name is empty. Dataset Creation aborted...");
		return;
	}
	if (recordingsTable->getItems().size() == 0) {
		m_errorMsg->showMessage("No Recording selected. Dataset Creation aborted...");
		return;
	}
	if (recordingsTable->getItems().size() == 0) {
		m_errorMsg->showMessage("Add at least one entity to the list...");
		return;
	}
	if (recordingsTable->getItems().size() == 0) {
		m_errorMsg->showMessage("Add at least one recording...");
		return;
	}
	if (entitiesItemList->getItems().size() == 0) {
		m_errorMsg->showMessage("Add at least one entity...");
		return;
	}
	if (keypointsItemList->getItems().size() == 0) {
		m_errorMsg->showMessage("Add at least one keypoint...");
		return;
	}
	if (!checkDatasetExists(m_datasetConfig->datasetPath + "/" + m_datasetConfig->datasetName)) {
		return;
	}

	emit createDataset(recordingsTable->getItems(), entitiesItemList->getItems(), keypointsItemList->getItems());
	datasetProgressInfoWindow->exec();
}

void NewDatasetWindow::datasetCreatedSot() {
	datasetProgressInfoWindow->accept();
}

void NewDatasetWindow::datasetCreationFailedSlot(QString errorMsg) {
	m_errorMsg->showMessage(errorMsg + "\nDataset Creation aborted...");
}


void NewDatasetWindow::savePresetsClickedSlot() {
	savePresetsWindow->updateListSlot();
	savePresetsWindow->show();
}


void NewDatasetWindow::loadPresetsClickedSlot() {
	loadPresetsWindow->updateListSlot();
	loadPresetsWindow->show();
}


void NewDatasetWindow::savePresetSlot(const QString& preset) {
	settings->beginGroup(preset);
	settings->beginGroup("entitiesItemList");
	QList<QString> entItemsList = entitiesItemList->getItems();
	settings->setValue("itemsList", QVariant::fromValue(entItemsList));
	settings->endGroup();
	settings->beginGroup("keypointItemList");
	QList<QString> keyItemsList = keypointsItemList->getItems();
	settings->setValue("itemsList", QVariant::fromValue(keyItemsList));
	settings->endGroup();
	settings->endGroup();
}


void NewDatasetWindow::loadPresetSlot(const QString& preset) {
	entitiesItemList->clear();
	keypointsItemList->clear();
	settings->beginGroup(preset);
	settings->beginGroup("entitiesItemList");
	QList<QString>entIitemsList = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : entIitemsList) {
		entitiesItemList->addItem(item);
	}
	settings->endGroup();
	settings->beginGroup("keypointItemList");
	QList<QString> keyItemsList = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : keyItemsList) {
		keypointsItemList->addItem(item);
	}
	settings->endGroup();
	settings->endGroup();
}

bool NewDatasetWindow::checkDatasetExists(const QString &path) {
	if (QFile::exists(path)) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "", "Dataset already exists! Continue anyways?",
	                                QMessageBox::Yes|QMessageBox::No);
	  if (reply == QMessageBox::No) {
	    return false;
	  }
	}
	return true;
}
