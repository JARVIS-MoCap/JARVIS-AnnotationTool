/*****************************************************************
	* File:			  newcalibrationwidget.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "newcalibrationwidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QDirIterator>
#include <QThread>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollArea>


NewCalibrationWidget::NewCalibrationWidget(QWidget *parent) : QWidget(parent) {
	this->setMinimumSize(600,600);
	settings = new QSettings();
	QGridLayout *layout = new QGridLayout(this);
	layout->setVerticalSpacing(20);
	layout->setHorizontalSpacing(100);
	m_errorMsg = new QErrorMessage();

	m_calibrationConfig = new CalibrationConfig;
	calibrationTool = new CalibrationTool(m_calibrationConfig);
	QThread *thread = new QThread;
	calibrationTool->moveToThread(thread);
	thread->start();
	connect(this, &NewCalibrationWidget::makeCalibrationSet, calibrationTool, &CalibrationTool::makeCalibrationSet);

	loadPresetsWindow = new PresetsWindow(&presets, "load", "New Calibration Widget/", this);
	savePresetsWindow = new PresetsWindow(&presets, "save", "New Calibration Widget/", this);
	connect(loadPresetsWindow, SIGNAL(loadPreset(QString)), this, SLOT(loadPresetSlot(QString)));
	connect(savePresetsWindow, SIGNAL(savePreset(QString)), this, SLOT(savePresetSlot(QString)));

	QLabel *newCalibrationLabel = new QLabel("New Calibration");
	newCalibrationLabel->setFont(QFont("Sans Serif", 18, QFont::Bold));

	QGroupBox *configurationBox = new QGroupBox("Configuration", this);
	configurationBox->setMaximumSize(800,9999);
	QScrollArea *configScrollArea = new QScrollArea(configurationBox);
	configScrollArea->setFrameShape(QFrame::NoFrame);
	QWidget *configWidget = new QWidget(configScrollArea);
	configWidget->setObjectName("configWidget");
	configWidget->setStyleSheet("QWidget#configWidget{background-color: rgba(66,66,66,50%)}");
	QGridLayout *configurationlayout = new QGridLayout(configWidget);
	QGridLayout *configurationlayoutout = new QGridLayout(configurationBox);
	configurationlayoutout->setContentsMargins(3,3,3,3);
	configurationlayout->setContentsMargins(7,7,7,7);
	configurationlayoutout->addWidget(configScrollArea);
	configScrollArea->setWidgetResizable(true);
	configScrollArea->setWidget(configWidget);

	QLabel *generalLabel = new QLabel("General");
	generalLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	QWidget *generalWidget = new QWidget(configWidget);
	QGridLayout *generallayout = new QGridLayout(generalWidget);
	generallayout->setContentsMargins(0,0,0,0);
	LabelWithToolTip *calibrationSetNameLabel = new LabelWithToolTip("  Calibration Set Name", "");
	calibrationSetNameEdit = new QLineEdit("New Calibration Set");
	LabelWithToolTip *calibrationSetPathLabel = new LabelWithToolTip("  Calibration Set Savepath", "");
	calibrationSetPathWidget = new DirPathWidget("Select Calibration Set Savepath");
	calibrationSetPathWidget->setPlaceholderText("Select a Path...");
	LabelWithToolTip *seperateIntrinsicsLabel = new LabelWithToolTip("  Seperate Recordings for Intrinsics", "Select 'Yes' if you have seperate single-camera recordings that you want to use for calibrating the intrinsics");
	seperateRadioWidget = new YesNoRadioWidget(generalWidget);
	seperateRadioWidget->setState(true);
	connect(seperateRadioWidget, &YesNoRadioWidget::stateChanged, this, &NewCalibrationWidget::sperateRadioStateChangedSlot);
	LabelWithToolTip *intrinsicsPathLabel = new LabelWithToolTip("  Intrinsics Folder Path", "");
	intrinsicsPathWidget = new DirPathWidget("Select Intrinsics Path");
	intrinsicsPathWidget->setPlaceholderText("Select a Path...");
	connect(intrinsicsPathWidget, &DirPathWidget::pathChanged, this, &NewCalibrationWidget::intrinsicsPathChangedSlot);
	LabelWithToolTip *extrinsicsPathLabel = new LabelWithToolTip("  Extrinsics Folder Path", "");
	extrinsicsPathWidget = new DirPathWidget("Select Extrinsics Path");
	extrinsicsPathWidget->setPlaceholderText("Select a Path...");
	updateNamesListButton = new QPushButton("Update Camera Names",this);
	updateNamesListButton->setMinimumSize(30,40);
	updateNamesListButton->setIcon(QIcon::fromTheme("update"));
	connect(updateNamesListButton, &QPushButton::clicked, this, &NewCalibrationWidget::updateNamesListSlot);

	QLabel *checkerBoardLabel = new QLabel("Checkerboard Layout");
	checkerBoardLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	int i = 0;
	generallayout->addWidget(calibrationSetNameLabel,i,0);
	generallayout->addWidget(calibrationSetNameEdit,i++,1);
	generallayout->addWidget(calibrationSetPathLabel,i,0);
	generallayout->addWidget(calibrationSetPathWidget,i++,1);
	generallayout->addWidget(seperateIntrinsicsLabel,i,0);
	generallayout->addWidget(seperateRadioWidget,i++,1);
	generallayout->addWidget(intrinsicsPathLabel,i,0);
	generallayout->addWidget(intrinsicsPathWidget,i++,1);
	generallayout->addWidget(extrinsicsPathLabel,i,0);
	generallayout->addWidget(extrinsicsPathWidget,i++,1);
	generallayout->addWidget(updateNamesListButton,i++,0,1,2, Qt::AlignRight);
	QWidget *generalSpacer = new QWidget(configWidget);
	generalSpacer->setMinimumSize(0,20);

	QLabel *calibParamsLabel = new QLabel("Calibration Settings");
	calibParamsLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	QWidget *calibParamsWidget = new QWidget(configWidget);
	QGridLayout *calibparamslayout = new QGridLayout(calibParamsWidget);
	calibparamslayout->setContentsMargins(0,0,0,0);
	LabelWithToolTip *intrinsicsFramesLabel = new LabelWithToolTip("  Max. Number of Frames for Intrinsics Calibration",
				"A good value is around 20-30. Note that the calibrator will only use as many checkerboards as it can detect.");
	intrinsicsFramesEdit = new QSpinBox(calibParamsWidget);
	intrinsicsFramesEdit->setRange(0,999);
	intrinsicsFramesEdit->setValue(20);
	LabelWithToolTip *extrinsicsFramesLabel = new LabelWithToolTip("  Max. Number of  Frames for Extrinsics Calibration",
			"A good value is around 20-30. Note that the calibrator will only use as many checkerboards as it can detect.");
	extrinsicsFramesEdit = new QSpinBox(calibParamsWidget);
	extrinsicsFramesEdit->setRange(0,999);
	extrinsicsFramesEdit->setValue(20);
	LabelWithToolTip *saveDebugLabel = new LabelWithToolTip("  Save Debug Images",
				"If you select 'Yes' a directory showing all detected checkerboards will be saved alongside the calibration parameters.");
	saveDebugRadioWidget = new YesNoRadioWidget(calibParamsWidget);
	saveDebugRadioWidget->setState(false);
	i = 0;
	calibparamslayout->addWidget(intrinsicsFramesLabel,i,0);
	calibparamslayout->addWidget(intrinsicsFramesEdit,i++,1);
	calibparamslayout->addWidget(extrinsicsFramesLabel,i,0);
	calibparamslayout->addWidget(extrinsicsFramesEdit,i++,1);
	calibparamslayout->addWidget(saveDebugLabel,i,0);
	calibparamslayout->addWidget(saveDebugRadioWidget,i++,1);
	QWidget *calibParamsSpacer = new QWidget(configWidget);
	calibParamsSpacer->setMinimumSize(0,20);

	QWidget *checkerboardWiget = new QWidget(configWidget);
	QGridLayout *checkerboardwidgetlayout = new QGridLayout(checkerboardWiget);
	checkerboardwidgetlayout->setContentsMargins(0,0,0,0);
	LabelWithToolTip *boardTypeLabel = new LabelWithToolTip("  Board Type",
				"ChAruco_Legacy only supports one specific pattern, use with care!");
	boardTypeCombo = new QComboBox(checkerboardWiget);
	boardTypeCombo->addItem("Standard");
	boardTypeCombo->addItem("ChArUco");
	boardTypeCombo->addItem("ChArUco_Legacy");
	connect(boardTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NewCalibrationWidget::checkerBoardPatternChangesSlot);
	connect(boardTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NewCalibrationWidget::boardTypeChangedSlot);
	charucoPatternLabel = new LabelWithToolTip("  ChArUco Pattern",
				"ArUco marker set used on your checkerboard!");
	charucoPatternCombo = new QComboBox(checkerboardWiget);
	charucoPatternCombo->addItems({"DICT_4X4_50","DICT_4X4_100","DICT_4X4_250",
				"DICT_4X4_1000","DICT_5X5_50","DICT_5X5_100","DICT_5X5_250",
				"DICT_5X5_1000","DICT_6X6_50","DICT_6X6_100","DICT_6X6_250",
				"DICT_6X6_1000","DICT_7X7_50","DICT_7X7_100","DICT_7X7_250",
				"DICT_7X7_1000","DICT_ARUCO_ORIGINAL","DICT_APRILTAG_16h5",
				"DICT_APRILTAG_25h9","DICT_APRILTAG_36h10","DICT_APRILTAG_36h11"});
	charucoPatternCombo->setCurrentIndex(8);
	charucoPatternCombo->hide();
	charucoPatternLabel->hide();

	LabelWithToolTip *widthLabel = new LabelWithToolTip("  Pattern Width",
				"Does not count the outer edge of the pattern. Make sure the visualization matches your board!");
	widthEdit = new QSpinBox();
	widthEdit->setRange(0,35);
	widthEdit->setValue(9);
	connect(widthEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &NewCalibrationWidget::checkerBoardPatternChangesSlot);
	LabelWithToolTip *heightLabel = new LabelWithToolTip("  Pattern Height",
			"Does not count the outer edge of the pattern. Make sure the visualization matches your board!");
	heightEdit = new QSpinBox();
	heightEdit->setRange(0,35);
	heightEdit->setValue(6);
	connect(heightEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &NewCalibrationWidget::checkerBoardPatternChangesSlot);
	LabelWithToolTip *sideLengthLabel = new LabelWithToolTip("  Side Length [mm]",
			"Sidelength of a single checkerboard square. Make sure you measure this accurately!");
	sideLengthEdit = new QDoubleSpinBox();
	sideLengthEdit->setRange(0.0,1000.0);
	sideLengthEdit->setValue(26.7);
	markerLengthLabel = new LabelWithToolTip("  ArUco Marker Length [mm]",
			"Sidelength of a single ArUco Marker square. Make sure you measure this accurately!");
	markerLengthEdit = new QDoubleSpinBox();
	markerLengthEdit->setRange(0.0,1000.0);
	markerLengthEdit->setValue(10.0);
	markerLengthEdit->hide();

	checkerBoardPreviewBox = new QGroupBox(this);
	QGridLayout *checkerboardpreviewlayout = new QGridLayout(checkerBoardPreviewBox);
	checkerboardpreviewlayout->setContentsMargins(0,0,0,0);
	checkerBoardPreviewBox->setStyleSheet("QGroupBox {  border: 4px solid palette(highlight);}");
	checkerBoardPreview = new QLabel(this);
	checkerBoardPreviewLabel = new QLabel("<font color=#64a420>Make sure this matches your Checkerboard!</font>");
	checkerBoardPreviewLabel->setFont(QFont("Sans Serif", 10));
	checkerboardpreviewlayout->addWidget(checkerBoardPreview,0,0, Qt::AlignCenter);
	checkerBoardPatternChangesSlot(0);


	i = 0;
	checkerboardwidgetlayout->addWidget(boardTypeLabel,i,0);
	checkerboardwidgetlayout->addWidget(boardTypeCombo,i++,1);
	checkerboardwidgetlayout->addWidget(charucoPatternLabel,i,0);
	checkerboardwidgetlayout->addWidget(charucoPatternCombo,i++,1);
	checkerboardwidgetlayout->addWidget(widthLabel,i,0);
	checkerboardwidgetlayout->addWidget(widthEdit,i++,1);
	checkerboardwidgetlayout->addWidget(heightLabel,i,0);
	checkerboardwidgetlayout->addWidget(heightEdit,i++,1);
	checkerboardwidgetlayout->addWidget(sideLengthLabel,i,0);
	checkerboardwidgetlayout->addWidget(sideLengthEdit,i++,1);
	checkerboardwidgetlayout->addWidget(markerLengthLabel,i,0);
	checkerboardwidgetlayout->addWidget(markerLengthEdit,i++,1);
	checkerboardwidgetlayout->addWidget(checkerBoardPreviewBox,i++,1, Qt::AlignCenter);
	checkerboardwidgetlayout->addWidget(checkerBoardPreviewLabel,i++,1, Qt::AlignCenter);


	QWidget *bottomSpacer = new QWidget(configWidget);
	bottomSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	i = 0;
	configurationlayout->addWidget(generalLabel,i++,0,1,2);
	configurationlayout->addWidget(generalWidget,i++,0,1,2);
	configurationlayout->addWidget(generalSpacer,i++,0,1,2);
	configurationlayout->addWidget(calibParamsLabel,i++,0,1,2);
	configurationlayout->addWidget(calibParamsWidget,i++,0,1,2);
	configurationlayout->addWidget(calibParamsSpacer,i++,0,1,2);
	configurationlayout->addWidget(checkerBoardLabel,i++,0,1,2);
	configurationlayout->addWidget(checkerboardWiget,i++,0,1,2);
	configurationlayout->addWidget(bottomSpacer,i++,0,1,2);

	QGroupBox *camerasBox = new QGroupBox("Cameras", this);
	//camerasBox->setMinimumSize(1000,400);
	QGridLayout *cameraslayout = new QGridLayout(camerasBox);
	cameraslayout->setContentsMargins(0,0,0,0);
	cameraList = new CameraNamesList("Cameras");
	cameraslayout->addWidget(cameraList,0,0);

	QGroupBox *cameraPairsBox = new QGroupBox("Camera Pairs", this);
	QGridLayout *camerapairlayout = new QGridLayout(cameraPairsBox);
	camerapairlayout->setContentsMargins(0,0,0,0);
	extrinsicsPairList = new ExtrinsicsPairList("Extrinsic Pairs");
	camerapairlayout->addWidget(extrinsicsPairList,0,0);


	QWidget *buttonBarWidget = new QWidget(this);
	buttonBarWidget->setMaximumSize(100000,50);
	QGridLayout *buttonbarlayout = new QGridLayout(buttonBarWidget);
	saveButton = new QPushButton("Save Preset");
	saveButton->setIcon(QIcon::fromTheme("upload"));
	saveButton->setMinimumSize(40,40);
	connect(saveButton, &QPushButton::clicked, this, &NewCalibrationWidget::savePresetsClickedSlot);
	loadButton = new QPushButton("Load Preset");
	loadButton->setIcon(QIcon::fromTheme("download"));
	loadButton->setMinimumSize(40,40);
	connect(loadButton, &QPushButton::clicked, this, &NewCalibrationWidget::loadPresetsClickedSlot);
	QWidget *middleSpacer = new QWidget();
	middleSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	calibrateButton = new QPushButton("Calibrate");
	calibrateButton->setIcon(QIcon::fromTheme("start"));
	calibrateButton->setMinimumSize(40,40);
	connect(calibrateButton, &QPushButton::clicked, this, &NewCalibrationWidget::calibrateClickedSlot);
	buttonbarlayout->addWidget(saveButton, 0,0);
	buttonbarlayout->addWidget(loadButton,0,1);
	buttonbarlayout->addWidget(middleSpacer,0,2);
	buttonbarlayout->addWidget(calibrateButton,0,3);

	layout->addWidget(newCalibrationLabel,0,0,1,2);
	layout->addWidget(configurationBox,1,0,2,1);
	layout->addWidget(camerasBox, 1,1);
	layout->addWidget(cameraPairsBox,2,1);
	layout->addWidget(buttonBarWidget,3,0,1,2);

	connect(calibrationTool, &CalibrationTool::calibrationFinished, this, &NewCalibrationWidget::calibrationFinishedSlot);
	connect(calibrationTool, &CalibrationTool::calibrationCanceled, this, &NewCalibrationWidget::calibrationCanceledSlot);
	connect(calibrationTool, &CalibrationTool::calibrationError, this, &NewCalibrationWidget::calibrationErrorSlot);

	//Signal Relay:
	connect(cameraList, &CameraNamesList::itemsChanged, extrinsicsPairList, &ExtrinsicsPairList::cameraNamesChangedSlot);
}


//TODO: Make this function structured less terrible
void NewCalibrationWidget::updateNamesListSlot() {
	QList<QString> detectedCams;
	if (seperateRadioWidget->state()) {
		const QString &intrinsicsPath = intrinsicsPathWidget->path();
		for (QDirIterator it(intrinsicsPath); it.hasNext();) {
			QString subpath = it.next();
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				if (checkIsValidRecording(intrinsicsPath, suffix.split(".").takeFirst())) {
					detectedCams.append(suffix.split(".").takeFirst());
				}
			}
		}
	}
	else {
		QString extrinsicsPath = extrinsicsPathWidget->path();
		for (QDirIterator it(extrinsicsPath); it.hasNext();) {
			QString subpath = it.next();
			if (m_validRecordingFormats.contains(subpath.split('.').takeLast())) {
				m_calibrationConfig->single_primary = true;
				break;
			}
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				for (QDirIterator subit(extrinsicsPath + "/" + suffix); subit.hasNext();) {
					QString subsubpath = subit.next().split('/').takeLast();
					if (checkIsValidRecording(subpath, subsubpath.split(".").takeFirst())) {
						if (detectedCams.count(subsubpath.split(".").takeFirst()) == 0) {
							detectedCams.append(subsubpath.split(".").takeFirst());
						}
					}
				}
			}
		}
		if (m_calibrationConfig->single_primary == true) {
			for (QDirIterator it(extrinsicsPath); it.hasNext();) {
				QString subpath = it.next();
				QString suffix = subpath.split('/').takeLast();
				if (suffix != "." && suffix != "..") {
					if (checkIsValidRecording(extrinsicsPath, suffix.split(".").takeFirst())) {
						detectedCams.append(suffix.split(".").takeFirst());
					}
				}
			}
		}
	}
	if (detectedCams.size() != 0) {
		NameSuggestionDialog *nameSuggestionDialog = new NameSuggestionDialog("Cameras", detectedCams,this);
		nameSuggestionDialog->exec();
		if(nameSuggestionDialog->result() == 1) {
			cameraList->clear();
			for (const auto& name : detectedCams) {
				cameraList->addItem(name);
			}
		}
		cameraList->addCameras(detectedCams);
		extrinsicsPairList->cameraNamesChangedSlot(cameraList->getItems());
	}
	QList<QList<QString>> detectedPairs;
	QString extrinsicsPath = extrinsicsPathWidget->path();
	if (m_calibrationConfig->single_primary == false) {
		for (QDirIterator it(extrinsicsPath); it.hasNext();) {
			QString subpath = it.next();
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				QString cam1 = suffix.split("-").takeFirst();
				QString cam2 = suffix.split("-").takeLast();
				QList<QString> cameraPair = {cam1, cam2};
				detectedPairs.append(cameraPair);
			}
		}
	}
	else {
		QString primary  = QInputDialog::getItem(this, "Select Primary Camera", "Primary:", detectedCams, 0, false);
		for (const auto& cam :  detectedCams) {
			if (cam != primary) {
				QList<QString> cameraPair = {primary, cam};
				detectedPairs.append(cameraPair);
			}
		}

	}
	if (detectedPairs.size() != 0) {
		QList<QString> pairNames;
		for (const auto &pair : detectedPairs) {
			pairNames.append(pair[0] + " --> " + pair[1]);
		}
		NameSuggestionDialog *nameSuggestionDialog = new NameSuggestionDialog("Camera Pairs", pairNames,this);
		nameSuggestionDialog->exec();
		if(nameSuggestionDialog->result() == 1) {
			extrinsicsPairList->setItems(detectedPairs);
		}
	}
}


void NewCalibrationWidget::calibrateClickedSlot() {
	QString intrinsicsPath = intrinsicsPathWidget->path();
	QString extrinsicsPath = extrinsicsPathWidget->path();
	QList<QString> cameraNames = cameraList->getItems();
	QList<QList<QString>> cameraPairs = extrinsicsPairList->getItems();
	if (calibrationSetPathWidget->path() == "") {
		m_errorMsg->showMessage("Please enter a savepath!");
		return;
	}

	if (calibrationSetNameEdit->text() == "") {
		m_errorMsg->showMessage("Please enter a name for the calibrarion set!");
		return;
	}

	if (cameraNames.size() == 0) {
		m_errorMsg->showMessage("Camera List is empty, aborting Calibration");
		return;
	}

	if (cameraPairs.size() == 0) {
		m_errorMsg->showMessage("No Camera Pairs defined, aborting Calibration.");
		return;
	}

	QString errorMsg;
	if (seperateRadioWidget->state()) {
		if (!checkIntrinsics(intrinsicsPath, errorMsg)) {
			m_errorMsg->showMessage(errorMsg);
			return;
		}
	}

	if (!checkExtrinsics(extrinsicsPath, errorMsg)) {
		m_errorMsg->showMessage(errorMsg);
		return;
	}


	m_calibrationConfig->calibrationSetName = calibrationSetNameEdit->text();
	m_calibrationConfig->calibrationSetPath = calibrationSetPathWidget->path();
	m_calibrationConfig->seperateIntrinsics = seperateRadioWidget->state();
	m_calibrationConfig->intrinsicsPath = intrinsicsPathWidget->path();
	m_calibrationConfig->extrinsicsPath = extrinsicsPathWidget->path();
	m_calibrationConfig->framesForIntrinsics = intrinsicsFramesEdit->value();
	m_calibrationConfig->framesForExtrinsics = extrinsicsFramesEdit->value();
	m_calibrationConfig->debug = saveDebugRadioWidget->state();
	m_calibrationConfig->boardType = boardTypeCombo->currentText();
	m_calibrationConfig->charucoPatternIdx = charucoPatternCombo->currentIndex();
	m_calibrationConfig->patternWidth = widthEdit->value();
	m_calibrationConfig->patternHeight = heightEdit->value();
	m_calibrationConfig->patternSideLength = sideLengthEdit->value();
	m_calibrationConfig->markerSideLength = markerLengthEdit->value();
	m_calibrationConfig->cameraNames = cameraList->getItems();
	m_calibrationConfig->cameraPairs = extrinsicsPairList->getItems();

	if (m_calibrationConfig->patternSideLength <= m_calibrationConfig->markerSideLength) {
		m_errorMsg->showMessage("Marker size cannot be bigger than pattern size!");
		return;
	}

	if (!checkCalibrationExists(m_calibrationConfig->calibrationSetPath + "/" + m_calibrationConfig->calibrationSetName)) {
		return;
	}

	if (!checkCheckerboard()) {
		m_errorMsg->showMessage("Your Checkerboard is symmetric, make sure you use an asymmetric checkerboard (see calibration guide).");
		return;
	}

	calibrationProgressInfoWindow = new CalibrationProgressInfoWindow(m_calibrationConfig->cameraNames, m_calibrationConfig->cameraPairs, this);
	connect(calibrationTool, &CalibrationTool::intrinsicsProgress, calibrationProgressInfoWindow, &CalibrationProgressInfoWindow::updateIntrinsicsProgressSlot);
	connect(calibrationTool, &CalibrationTool::extrinsicsProgress, calibrationProgressInfoWindow, &CalibrationProgressInfoWindow::updateExtrinsicsProgressSlot);
	connect(calibrationProgressInfoWindow, &CalibrationProgressInfoWindow::rejected, calibrationTool, &CalibrationTool::cancelCalibrationSlot);
	emit makeCalibrationSet();
	calibrationProgressInfoWindow->exec();
}


void NewCalibrationWidget::calibrationFinishedSlot() {
	calibrationProgressInfoWindow->accept();
	delete calibrationProgressInfoWindow;
	QMap<int, double> intrinsicsReproErrors = calibrationTool->getIntrinsicsReproErrors();
	QMap<int, double> extrinsicsReproErrors = calibrationTool->getExtrinsicsReproErrors();
	CalibrationStatisticsWindow *calibrationStatisticsWindow = new CalibrationStatisticsWindow(m_calibrationConfig->cameraNames, m_calibrationConfig->cameraPairs, intrinsicsReproErrors,  extrinsicsReproErrors,this);
	calibrationStatisticsWindow->exec();
}


void NewCalibrationWidget::calibrationCanceledSlot() {
	calibrationProgressInfoWindow->accept();
	delete calibrationProgressInfoWindow;
}


void NewCalibrationWidget::calibrationErrorSlot(const QString &errorMsg) {
	m_errorMsg->showMessage("Calibration unsuccessful! " + errorMsg);
	m_errorMsg->resize(500,200);
	calibrationProgressInfoWindow->accept();
	delete calibrationProgressInfoWindow;
}


bool NewCalibrationWidget::checkIntrinsics(const QString& path, QString &errorMsg) {
	QList<QString> cameraNames = cameraList->getItems();
	bool allFilesValid = true;
	for (const auto & cam : cameraNames) {
		if (!checkIsValidRecording(path, cam)) {
			errorMsg = "Recording for camera \"" + cam + "\" not found.";
			allFilesValid = false;
		}
	}
	return allFilesValid;
}


bool NewCalibrationWidget::checkExtrinsics(const QString& path, QString & errorMsg) {
	QList<QList<QString>> cameraPairs = extrinsicsPairList->getItems();
	bool allFilesValid = true;

	for (const auto & pair : cameraPairs) {
		if (pair.size() == 2) {
			if (m_calibrationConfig->single_primary == true) {
				if (!checkIsValidRecording(path, pair[0])) {
					errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
				if (!checkIsValidRecording(path, pair[1])) {
					errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
			}
			else {
				if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[0])) {
					errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
				if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[1])) {
					errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
			}
		}
		else if (pair.size() == 3) {
			if (m_calibrationConfig->single_primary == true) {
				if (!checkIsValidRecording(path, pair[0]))
				{
					errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
				if (!checkIsValidRecording(path, pair[1]))
				{
					errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
				if (!checkIsValidRecording(path, pair[2]))
				{
					errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
			}
			else {
				if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[0]))
				{
					errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
					allFilesValid = false;
				}
				if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[1]))
				{
					if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[0]))
					{
						errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
						allFilesValid = false;
					}
					if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[1]))
					{
						errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
						allFilesValid = false;
					}
					allFilesValid = false;
				}
				if (!checkIsValidRecording(path + "/" + pair[1] + "-" + pair[2], pair[2]))
				{
					errorMsg = "Recording for secondary camera in pair \"" + pair[1] + " --> " + pair[2] + "\" not found.";
					allFilesValid = false;
				}
			}
		}
	}
	return allFilesValid;
}


bool NewCalibrationWidget::checkCheckerboard() {
	if (m_calibrationConfig->boardType == "ChArUco_Legacy" || m_calibrationConfig->boardType == "ChArUco" || m_calibrationConfig->patternWidth%2 + m_calibrationConfig->patternHeight%2 == 1) {
		return true;
	}
	else {
		return false;
	}
}


bool NewCalibrationWidget::checkCalibrationExists(const QString &path) {
	if (QFile::exists(path)) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "", "Calibration already exists! Continue anyway?",
	                                QMessageBox::Yes|QMessageBox::No);
	  if (reply == QMessageBox::No) {
	    return false;
	  }
	}
	return true;
}


bool NewCalibrationWidget::checkIsValidRecording(const QString& path, const QString& recording) {
	bool validFileFound = false;
	for (const auto& format : m_validRecordingFormats) {
		if (QFile::exists(path + "/" + recording  + "." + format)) validFileFound = true;
	}
	return validFileFound;
}


void NewCalibrationWidget::sperateRadioStateChangedSlot(bool state) {
	if (state) {
		intrinsicsPathWidget->setEnabled(true);
	}
	else {
		intrinsicsPathWidget->setEnabled(false);
	}
}


QImage NewCalibrationWidget::createCheckerboardPreview() {
	int width, height;
	if (boardTypeCombo->currentText() == "ChArUco") {
		width = widthEdit->value()-1;
		height = heightEdit->value()-1;
	}
	else {
		width = widthEdit->value();
		height = heightEdit->value();
	}

	QImage checkerBoardImage(width+1, height+1, QImage::Format_RGB888);
	for (int i = 0; i < width+1; i++) {
		for (int j = 0; j < height+1; j++) {
			if ((i+j)%2 == 0) {
				checkerBoardImage.setPixelColor(i,j,QColor(0,0,0));
			}
			else {
				checkerBoardImage.setPixelColor(i,j,QColor(255,255,255));
			}
		}
	}
	return checkerBoardImage;
}


void NewCalibrationWidget::checkerBoardPatternChangesSlot(int val) {
	Q_UNUSED(val);
	if (boardTypeCombo->currentText() == "ChArUco") {
		checkerBoardPreview->setPixmap(QPixmap::fromImage(createCheckerboardPreview().scaled((widthEdit->value())*20,(heightEdit->value())*20)));
	}
	else {
		checkerBoardPreview->setPixmap(QPixmap::fromImage(createCheckerboardPreview().scaled((widthEdit->value()+1)*20,(heightEdit->value()+1)*20)));
	}
	if (boardTypeCombo->currentText() == "Standard" && (widthEdit->value()+ heightEdit->value()) % 2 == 0) {
		checkerBoardPreviewBox->setStyleSheet("QGroupBox {  border: 4px solid rgba(164,32,34,255);}");
		checkerBoardPreviewLabel->setText("<font color=#a42022>Use an assymetric Checkerboard!</font>");
	}
	else {
		checkerBoardPreviewBox->setStyleSheet("QGroupBox {  border: 4px solid palette(highlight);}");
		checkerBoardPreviewLabel->setText("<font color=#64a420>Make sure this matches your Checkerboard Pattern!</font>");
	}
}

void NewCalibrationWidget::boardTypeChangedSlot(int val) {
	Q_UNUSED(val);
	if (boardTypeCombo->currentText() == "ChArUco") {
		charucoPatternCombo->show();
		charucoPatternLabel->show();
		markerLengthEdit->show();
		markerLengthLabel->show();
	}
	else {
		charucoPatternCombo->hide();
		charucoPatternLabel->hide();
		markerLengthEdit->hide();
		markerLengthLabel->hide();
	}
}


void NewCalibrationWidget::intrinsicsPathChangedSlot(const QString &path) {
	if (extrinsicsPathWidget->path() == "") {
		QDir intrinsicsDir = QDir(path);
		intrinsicsDir.cdUp();
		extrinsicsPathWidget->setDefaultPath(intrinsicsDir.path());
	}
}


void NewCalibrationWidget::savePresetsClickedSlot() {
	savePresetsWindow->updateListSlot();
	savePresetsWindow->exec();
}


void NewCalibrationWidget::loadPresetsClickedSlot() {
	loadPresetsWindow->updateListSlot();
	loadPresetsWindow->exec();
}


void NewCalibrationWidget::savePresetSlot(const QString& preset) {
	settings->beginGroup(preset);
	settings->beginGroup("cameraNames");
	QList<QString> cameraNames = cameraList->getItems();
	settings->setValue("itemsList", QVariant::fromValue(cameraNames));
	settings->endGroup();
	settings->beginGroup("cameraPairs");
	settings->setValue("itemsList", QVariant::fromValue(extrinsicsPairList->getItems()));
	settings->endGroup();
	settings->beginGroup("Configuration");
	settings->setValue("calibrationSetName", calibrationSetNameEdit->text());
	settings->setValue("calibrationSetPath", calibrationSetPathWidget->path());
	settings->setValue("seperateIntrinsics", seperateRadioWidget->state());
	settings->setValue("intrinsicsFolder", intrinsicsPathWidget->path());
	settings->setValue("extrinsicsFolder", extrinsicsPathWidget->path());
	settings->setValue("singlePrimary", m_calibrationConfig->single_primary);
	settings->setValue("intrinsicsFrames", intrinsicsFramesEdit->value());
	settings->setValue("extrinsicsFrames", extrinsicsFramesEdit->value());
	settings->setValue("saveDebugImages", saveDebugRadioWidget->state());
	settings->setValue("boardType", boardTypeCombo->currentIndex());
	settings->setValue("charucoPattern", charucoPatternCombo->currentIndex());
	settings->setValue("patternWidth", widthEdit->value());
	settings->setValue("patternHeight", heightEdit->value());
	settings->setValue("sideLength", sideLengthEdit->value());
	settings->setValue("markerLength", markerLengthEdit->value());
	settings->endGroup();
	settings->endGroup();
}


void NewCalibrationWidget::loadPresetSlot(const QString& preset) {
	cameraList->clear();
	settings->beginGroup(preset);
	settings->beginGroup("cameraNames");
	QList<QString>cameraNames = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : cameraNames) {
		cameraList->addItem(item);
	}
	cameraList->addCameras(cameraNames);
	settings->endGroup();
	settings->beginGroup("cameraPairs");
	extrinsicsPairList->setItems(settings->value("itemsList").value<QList<QList<QString>>>());
	settings->endGroup();
	settings->beginGroup("Configuration");
	if (settings->contains("calibrationSetName")) {
		calibrationSetNameEdit->setText(settings->value("calibrationSetName").toString());
	}
	calibrationSetPathWidget->setPath(settings->value("calibrationSetPath").toString());
	seperateRadioWidget->setState(settings->value("seperateIntrinsics").toBool());
	intrinsicsPathWidget->setPath(settings->value("intrinsicsFolder").toString());
	extrinsicsPathWidget->setPath(settings->value("extrinsicsFolder").toString());
	m_calibrationConfig->single_primary = settings->value("singlePrimary").toBool();
	boardTypeCombo->setCurrentIndex(settings->value("boardType").toInt());
	charucoPatternCombo->setCurrentIndex(settings->value("charucoPattern").toInt());

	intrinsicsFramesEdit->setValue(settings->value("intrinsicsFrames").toInt());
	extrinsicsFramesEdit->setValue(settings->value("extrinsicsFrames").toInt());
	saveDebugRadioWidget->setState(settings->value("saveDebugImages").toBool());

	widthEdit->setValue(settings->value("patternWidth").toInt());
	heightEdit->setValue(settings->value("patternHeight").toInt());
	sideLengthEdit->setValue(settings->value("sideLength").toDouble());
	markerLengthEdit->setValue(settings->value("markerLength").toDouble());

	settings->endGroup();
	settings->endGroup();
	extrinsicsPairList->cameraNamesChangedSlot(cameraList->getItems());
}
