/*****************************************************************
 * File:			newcalibrationwidget.cpp
 * Created: 	30. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "newcalibrationwidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QDirIterator>
#include <QThread>


NewCalibrationWidget::NewCalibrationWidget(QWidget *parent) : QWidget(parent) {
	this->setMinimumSize(600,600);
	settings = new QSettings();
	QGridLayout *layout = new QGridLayout(this);
	layout->setVerticalSpacing(20);
	layout->setHorizontalSpacing(100);
	m_errorMsg = new QErrorMessage();

	loadPresetsWindow = new PresetsWindow(&presets, "load", "New Calibration Widget/");
	savePresetsWindow = new PresetsWindow(&presets, "save", "New Calibration Widget/");
	connect(loadPresetsWindow, SIGNAL(loadPreset(QString)), this, SLOT(loadPresetSlot(QString)));
	connect(savePresetsWindow, SIGNAL(savePreset(QString)), this, SLOT(savePresetSlot(QString)));

	QLabel *newCalibrationLabel = new QLabel("New Calibration");
	newCalibrationLabel->setFont(QFont("Sans Serif", 18, QFont::Bold));

	QGroupBox *configurationBox = new QGroupBox("Configuration", this);
	QGridLayout *configurationlayout = new QGridLayout(configurationBox);

	QLabel *generalLabel = new QLabel("General");
	QWidget *generalWidget = new QWidget(configurationBox);
	QGridLayout *generallayout = new QGridLayout(generalWidget);
	generallayout->setMargin(0);
	generalLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	LabelWithToolTip *seperateIntrinsicsLabel = new LabelWithToolTip("  Seperate Recordings for Intrinsics");
	seperateRadioWidget = new YesNoRadioWidget(generalWidget);
	seperateRadioWidget->setState(true);
	LabelWithToolTip *calibrateIntrinsicsLabel = new LabelWithToolTip("  Calibrate Intrinsics");
	calibrateIntrinsicsRadioWidget = new YesNoRadioWidget(generalWidget);
	calibrateIntrinsicsRadioWidget->setState(true);
	LabelWithToolTip *CalibrateExtrinsicsLabel = new LabelWithToolTip("  Calibrate Extrinsics");
	calibrateExtrinsicsRadioWidget = new YesNoRadioWidget(generalWidget);
	calibrateExtrinsicsRadioWidget->setState(true);
	LabelWithToolTip *intrinsicsPathLabel = new LabelWithToolTip("  Intrinsics Folder Path");
	intrinsicsPathWidget = new DirPathWidget("  Select Intrinsics Path");
	//make sure it emits edited signal when changing text
	LabelWithToolTip *extrinsicsPathLabel = new LabelWithToolTip("  Extrinsics Folder Path");
	extrinsicsPathWidget = new DirPathWidget("  Select Extrinsics Path");
	QLabel *checkerBoardLabel = new QLabel("Checkerboard Layout");
	checkerBoardLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	int i = 0;
	generallayout->addWidget(seperateIntrinsicsLabel,i,0);
	generallayout->addWidget(seperateRadioWidget,i++,1);
	generallayout->addWidget(calibrateIntrinsicsLabel,i,0);
	generallayout->addWidget(calibrateIntrinsicsRadioWidget,i++,1);
	generallayout->addWidget(CalibrateExtrinsicsLabel,i,0);
	generallayout->addWidget(calibrateExtrinsicsRadioWidget,i++,1);
	generallayout->addWidget(intrinsicsPathLabel,i,0);
	generallayout->addWidget(intrinsicsPathWidget,i++,1);
	generallayout->addWidget(extrinsicsPathLabel,i,0);
	generallayout->addWidget(extrinsicsPathWidget,i++,1);
	QWidget *generalSpacer = new QWidget(configurationBox);
	generalSpacer->setMinimumSize(0,20);

	QLabel *calibParamsLabel = new QLabel("Calibration Parameters");
	calibParamsLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	QWidget *calibParamsWidget = new QWidget(configurationBox);
	QGridLayout *calibparamslayout = new QGridLayout(calibParamsWidget);
	calibparamslayout->setMargin(0);
	LabelWithToolTip *intrinsicsFramesLabel = new LabelWithToolTip("  Frames for Intrinsics Calibration");
	intrinsicsFramesEdit = new QSpinBox(calibParamsWidget);
	intrinsicsFramesEdit->setRange(0,999);
	intrinsicsFramesEdit->setValue(100);
	LabelWithToolTip *extrinsicsFramesLabel = new LabelWithToolTip("  Frames for Extrinsics Calibration");
	extrinsicsFramesEdit = new QSpinBox(calibParamsWidget);
	extrinsicsFramesEdit->setRange(0,999);
	extrinsicsFramesEdit->setValue(100);
	i = 0;
	calibparamslayout->addWidget(intrinsicsFramesLabel,i,0);
	calibparamslayout->addWidget(intrinsicsFramesEdit,i++,1);
	calibparamslayout->addWidget(extrinsicsFramesLabel,i,0);
	calibparamslayout->addWidget(extrinsicsFramesEdit,i++,1);
	QWidget *calibParamsSpacer = new QWidget(configurationBox);
	calibParamsSpacer->setMinimumSize(0,20);

	QWidget *checkerboardWiget = new QWidget(configurationBox);
	QGridLayout *checkerboardwidgetlayout = new QGridLayout(checkerboardWiget);
	checkerboardwidgetlayout->setMargin(0);
	LabelWithToolTip *widthLabel = new LabelWithToolTip("  Pattern Width");
	widthEdit = new QSpinBox();
	widthEdit->setRange(0,20);
	widthEdit->setValue(9);
	LabelWithToolTip *heightLabel = new LabelWithToolTip("  Pattern Height");
	heightEdit = new QSpinBox();
	heightEdit->setRange(0,20);
	heightEdit->setValue(6);
	LabelWithToolTip *sideLengthLabel = new LabelWithToolTip("  Side Length [mm]");
	sideLengthEdit = new QDoubleSpinBox();
	sideLengthEdit->setRange(0.0,1000.0);
	sideLengthEdit->setValue(26.7);
	i = 0;
	checkerboardwidgetlayout->addWidget(widthLabel,i,0);
	checkerboardwidgetlayout->addWidget(widthEdit,i++,1);
	checkerboardwidgetlayout->addWidget(heightLabel,i,0);
	checkerboardwidgetlayout->addWidget(heightEdit,i++,1);
	checkerboardwidgetlayout->addWidget(sideLengthLabel,i,0);
	checkerboardwidgetlayout->addWidget(sideLengthEdit,i++,1);
	QWidget *bottomSpacer = new QWidget(configurationBox);
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
	cameraslayout->setMargin(0);
	cameraList = new ConfigurableItemList("Cameras");
	cameraslayout->addWidget(cameraList,0,0);

	QGroupBox *cameraPairsBox = new QGroupBox("Camera Pairs", this);
	QGridLayout *camerapairlayout = new QGridLayout(cameraPairsBox);
	camerapairlayout->setMargin(0);
	extrinsicsPairList = new ExtrinsicsPairList("Extrinsic Pairs");
	camerapairlayout->addWidget(extrinsicsPairList,0,0);


	QWidget *buttonBarWidget = new QWidget(this);
	buttonBarWidget->setMaximumSize(100000,50);
	QGridLayout *buttonbarlayout = new QGridLayout(buttonBarWidget);
	saveButton = new QPushButton("Save");
	saveButton->setIcon(QIcon::fromTheme("upload"));
	saveButton->setMinimumSize(40,40);
	connect(saveButton, &QPushButton::clicked, this, &NewCalibrationWidget::savePresetsClickedSlot);
	loadButton = new QPushButton("Load");
	loadButton->setIcon(QIcon::fromTheme("download"));
	loadButton->setMinimumSize(40,40);
	connect(loadButton, &QPushButton::clicked, this, &NewCalibrationWidget::loadPresetsClickedSlot);
	QWidget *middleSpacer = new QWidget();
	middleSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	calibrateButton = new QPushButton("Calibrate");
	calibrateButton->setMinimumSize(40,40);
	//connect(createButton, &QPushButton::clicked, this, &NewDatasetWindow::createDatasetClickedSlot);
	buttonbarlayout->addWidget(saveButton, 0,0);
	buttonbarlayout->addWidget(loadButton,0,1);
	buttonbarlayout->addWidget(middleSpacer,0,2);
	buttonbarlayout->addWidget(calibrateButton,0,3);

	layout->addWidget(newCalibrationLabel,0,0,1,2);
	layout->addWidget(configurationBox,1,0,2,1);
	layout->addWidget(camerasBox, 1,1);
	layout->addWidget(cameraPairsBox,2,1);
	layout->addWidget(buttonBarWidget,3,0,1,2);

	//Signal Relay:
	connect(cameraList, &ConfigurableItemList::itemsChanged, extrinsicsPairList, &ExtrinsicsPairList::cameraNamesChangedSlot);

}



void NewCalibrationWidget::savePresetsClickedSlot() {
	savePresetsWindow->updateListSlot();
	savePresetsWindow->show();
}


void NewCalibrationWidget::loadPresetsClickedSlot() {
	loadPresetsWindow->updateListSlot();
	loadPresetsWindow->show();
}


void NewCalibrationWidget::savePresetSlot(const QString& preset) {
	settings->beginGroup(preset);
	settings->beginGroup("cameraNames");
	QList<QString> cameraNames;
	for (const auto& item : cameraList->itemSelectorList->findItems("",Qt::MatchContains)) {
		cameraNames.append(item->text());
	}
	settings->setValue("itemsList", QVariant::fromValue(cameraNames));
	settings->endGroup();
	settings->beginGroup("cameraPairs");
	settings->setValue("itemsList", QVariant::fromValue(extrinsicsPairList->getItems()));
	settings->endGroup();
	settings->beginGroup("Configuration");
	std::cout << "State: " << seperateRadioWidget->state() << std::endl;
	settings->setValue("seperateIntrinsics", seperateRadioWidget->state());
	settings->setValue("calibrateIntrinsics", calibrateIntrinsicsRadioWidget->state());
	settings->setValue("calibrateExtrinsics", calibrateExtrinsicsRadioWidget->state());
	settings->setValue("intrinsicsFolder", intrinsicsPathWidget->path());
	settings->setValue("extrinsicsFolder", extrinsicsPathWidget->path());
	settings->setValue("intrinsicsFrames", intrinsicsFramesEdit->value());
	settings->setValue("extrinsicsFrames", extrinsicsFramesEdit->value());
	settings->setValue("patternWidth", widthEdit->value());
	settings->setValue("patternHeight", heightEdit->value());
	settings->setValue("sideLength", sideLengthEdit->value());
	settings->endGroup();
	settings->endGroup();
}


void NewCalibrationWidget::loadPresetSlot(const QString& preset) {
	cameraList->itemSelectorList->clear();
	settings->beginGroup(preset);
	settings->beginGroup("cameraNames");
	QList<QString>cameraNames = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : cameraNames) {
		cameraList->itemSelectorList->addItem(item);
	}
	settings->endGroup();
	settings->beginGroup("cameraPairs");
	extrinsicsPairList->setItems(settings->value("itemsList").value<QList<QList<QString>>>());
	settings->endGroup();
	settings->beginGroup("Configuration");
	std::cout << "State: " << settings->value("seperateIntrinsics").toBool() << std::endl;
	seperateRadioWidget->setState(settings->value("seperateIntrinsics").toBool());
	calibrateIntrinsicsRadioWidget->setState(settings->value("calibrateIntrinsics").toBool());
	calibrateExtrinsicsRadioWidget->setState(settings->value("calibrateExtrinsics").toBool());
	intrinsicsPathWidget->setPath(settings->value("intrinsicsFolder").toString());
	extrinsicsPathWidget->setPath(settings->value("extrinsicsFolder").toString());
	intrinsicsFramesEdit->setValue(settings->value("intrinsicsFrames").toInt());
	extrinsicsFramesEdit->setValue(settings->value("extrinsicsFrames").toInt());
	widthEdit->setValue(settings->value("patternWidth").toInt());
	heightEdit->setValue(settings->value("patternHeight").toInt());
	sideLengthEdit->setValue(settings->value("sideLength").toDouble());
	settings->endGroup();
	settings->endGroup();
}


YesNoRadioWidget::YesNoRadioWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);
	yesButton = new QRadioButton("Yes",this);
	connect(yesButton, &QRadioButton::toggled, this, &YesNoRadioWidget::stateChanged);
	noButton = new QRadioButton("No",this);
	layout->addWidget(yesButton,0,0);
	layout->addWidget(noButton,0,1);
}

void YesNoRadioWidget::setState(bool state) {
	if (state) {
		yesButton->setChecked(true);
	}
	else {
		noButton->setChecked(true);
	}
}
