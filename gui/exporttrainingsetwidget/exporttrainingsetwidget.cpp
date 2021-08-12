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
#include <QScrollArea>


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
	configurationBox->setMaximumSize(800,9999);
	QScrollArea *configScrollArea = new QScrollArea(configurationBox);
	configScrollArea->setFrameShape(QFrame::NoFrame);
	QWidget *configWidget = new QWidget(configScrollArea);
	configWidget->setObjectName("exportConfigWidget");
	configWidget->setStyleSheet("QWidget#exportConfigWidget{background-color: rgba(66,66,66,50%)}");
	QGridLayout *configurationlayout = new QGridLayout(configWidget);
	QGridLayout *configurationlayoutout = new QGridLayout(configurationBox);
	configurationlayoutout->setMargin(3);
	configurationlayout->setMargin(7);
	configurationlayoutout->addWidget(configScrollArea);
	configScrollArea->setWidgetResizable(true);
	configScrollArea->setWidget(configWidget);

	LabelWithToolTip *trainingsetNameLabel = new LabelWithToolTip("  Trainingset Name");
	trainingsetNameEdit = new QLineEdit(configWidget);
	trainingsetNameEdit->setText("New Trainingset");
	LabelWithToolTip *trainingsetPathLabel = new LabelWithToolTip("  Trainingset Savepath");
	trainingsetSavePathWidget = new DirPathWidget("Select Trainingset Savepath");
	QWidget *setupSpacer1 = new QWidget(configWidget);

	setupSpacer1->setMinimumSize(10,10);
	setupSpacer1->setMaximumSize(9999,10);
	LabelWithToolTip *trainingsetTypeLabel = new LabelWithToolTip("  Trainingset Type");
	QWidget *trainingsetTypeWidget = new QWidget(configWidget);
	QGridLayout *trainingsettypelayout = new QGridLayout(trainingsetTypeWidget);
	trainingsettypelayout->setMargin(0);
	type3DButton = new QRadioButton("3D",this);
	type3DButton->setChecked(true);
	type2DButton = new QRadioButton("2D",this);
	trainingsettypelayout->addWidget(type3DButton,0,1);
	trainingsettypelayout->addWidget(type2DButton,0,0);
	LabelWithToolTip *intrinsicsPathLabel = new LabelWithToolTip("  Intrinsics Folder Path");
	intrinsicsPathWidget = new DirPathWidget("Select Intrinsics Path");
	LabelWithToolTip *extrinsicsPathLabel = new LabelWithToolTip("  Extrinsics Folder Path");
	extrinsicsPathWidget = new DirPathWidget("Select Extrinsics Path");
	QWidget *setupSpacer2 = new QWidget(configWidget);

	setupSpacer2->setMinimumSize(10,10);
	setupSpacer2->setMaximumSize(9999,10);
	LabelWithToolTip *validationFractionLabel = new LabelWithToolTip("  Validation Fraction");
	validationFractionEdit = new QDoubleSpinBox(configWidget);
	validationFractionEdit->setRange(0.001,0.999);
	LabelWithToolTip *shuffleBeforeSplitLabel = new LabelWithToolTip("  Shuffle before Train/Validation Split");
	shuffleBeforeSplitWidget = new YesNoRadioWidget(configWidget);
	shuffleBeforeSplitWidget->setState(true);
	LabelWithToolTip *shuffleSeedLabel = new LabelWithToolTip("  Shuffle Seed");
	shuffleSeedEdit = new QSpinBox(configWidget);
	shuffleSeedEdit->setRange(0,99999);
	QWidget *configBottomSpacer = new QWidget(configWidget);
	configBottomSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	int i = 0;
	configurationlayout->addWidget(trainingsetNameLabel,i,0);
	configurationlayout->addWidget(trainingsetNameEdit,i++,1);
	configurationlayout->addWidget(trainingsetPathLabel,i,0);
	configurationlayout->addWidget(trainingsetSavePathWidget,i++,1);
	configurationlayout->addWidget(setupSpacer1,i++,1,1,2);
	configurationlayout->addWidget(trainingsetTypeLabel,i,0);
	configurationlayout->addWidget(trainingsetTypeWidget,i++,1);
	configurationlayout->addWidget(intrinsicsPathLabel,i,0);
	configurationlayout->addWidget(intrinsicsPathWidget,i++,1);
	configurationlayout->addWidget(extrinsicsPathLabel,i,0);
	configurationlayout->addWidget(extrinsicsPathWidget,i++,1);
	configurationlayout->addWidget(setupSpacer2,i++,1,1,2);
	configurationlayout->addWidget(validationFractionLabel,i,0);
	configurationlayout->addWidget(validationFractionEdit,i++,1);
	configurationlayout->addWidget(shuffleBeforeSplitLabel,i,0);
	configurationlayout->addWidget(shuffleBeforeSplitWidget,i++,1);
	configurationlayout->addWidget(shuffleSeedLabel,i,0);
	configurationlayout->addWidget(shuffleSeedEdit,i++,1);
	configurationlayout->addWidget(configBottomSpacer,i++,1,1,2);

	QGroupBox *summaryBox = new QGroupBox("Summary", this);
	summaryBox->setMaximumSize(800,9999);
	QScrollArea *summaryScrollArea = new QScrollArea(summaryBox);
	summaryScrollArea->setFrameShape(QFrame::NoFrame);
	QWidget *summaryWidget = new QWidget(summaryScrollArea);
	summaryWidget->setObjectName("exportConfigWidget");
	summaryWidget->setStyleSheet("QWidget#exportConfigWidget{background-color: rgba(66,66,66,50%)}");
	QGridLayout *summarylayout = new QGridLayout(summaryWidget);
	QGridLayout *summarylayoutout = new QGridLayout(summaryBox);
	summarylayoutout->setMargin(3);
	summarylayout->setMargin(7);
	summarylayoutout->addWidget(summaryScrollArea);
	summaryScrollArea->setWidgetResizable(true);
	summaryScrollArea->setWidget(summaryWidget);
	LabelWithToolTip *numberFramesTrainingLabel = new LabelWithToolTip("  Total Number Frames for Training Split");
	numberFramesTrainingEdit = new QLineEdit(summaryWidget);
	numberFramesTrainingEdit->setReadOnly(true);
	numberFramesTrainingEdit->setText(QString::number(6000));
	LabelWithToolTip *numberFramesValidationLabel = new LabelWithToolTip("  Total Number Frames for Validation Split");
	numberFramesValidationEdit = new QLineEdit(summaryWidget);
	numberFramesValidationEdit->setReadOnly(true);
	numberFramesValidationEdit->setText(QString::number(900));
	LabelWithToolTip *numberEntitiesLabel = new LabelWithToolTip("  Number of Entities");
	numberEntitiesEdit = new QLineEdit(summaryWidget);
	numberEntitiesEdit->setReadOnly(true);
	numberEntitiesEdit->setText(QString::number(1));
	LabelWithToolTip *numberKeypointsLabel = new LabelWithToolTip("  Number of Entities");
	numberKeypointsEdit = new QLineEdit(summaryWidget);
	numberKeypointsEdit->setReadOnly(true);
	numberKeypointsEdit->setText(QString::number(1));
	QList<QString> test;
	QList<int> test2;
	compositionChartView = new CompositionChartView(test, test2);
	QWidget *summaryBottomSpacer = new QWidget(configWidget);
	summaryBottomSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	summarylayout->addWidget(numberFramesTrainingLabel,i,0);
	summarylayout->addWidget(numberFramesTrainingEdit,i++,1);
	summarylayout->addWidget(numberFramesValidationLabel,i,0);
	summarylayout->addWidget(numberFramesValidationEdit,i++,1);
	summarylayout->addWidget(numberEntitiesLabel,i,0);
	summarylayout->addWidget(numberEntitiesEdit,i++,1);
	summarylayout->addWidget(numberKeypointsLabel,i,0);
	summarylayout->addWidget(numberKeypointsEdit,i++,1);
	summarylayout->addWidget(compositionChartView,i++,1);
	summarylayout->addWidget(summaryBottomSpacer,i++,1,1,2);


	QWidget *listWidget = new QWidget(this);
	QGridLayout *listlayout = new QGridLayout(listWidget);
	listlayout->setMargin(0);

	QGroupBox *datasetListBox = new QGroupBox("Datasets", this);
	QGridLayout *datasetlistlayout = new QGridLayout(datasetListBox);

	QGroupBox *entitiesListBox = new QGroupBox("Entities", this);
	QGridLayout *enitieslistlayout = new QGridLayout(entitiesListBox);

	QGroupBox *keypointListBox = new QGroupBox("Keypoints", this);
	QGridLayout *keypointlistlayout = new QGridLayout(keypointListBox);

	listlayout->addWidget(datasetListBox,0,0);
	listlayout->addWidget(entitiesListBox,1,0);
	listlayout->addWidget(keypointListBox,2,0);



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
	layout->addWidget(configurationBox,1,0);
	layout->addWidget(summaryBox,2,0);
	layout->addWidget(listWidget,1,1,2,1);
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
