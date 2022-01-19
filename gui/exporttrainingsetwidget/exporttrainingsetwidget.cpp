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

	trainingSetExporter = new TrainingSetExporter(m_datasetExportItems);
	QThread *thread = new QThread;
	trainingSetExporter->moveToThread(thread);
	thread->start();

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
	trainingsetSavePathWidget->setPlaceholderText("Select a Path...");
	QWidget *setupSpacer1 = new QWidget(configWidget);

	setupSpacer1->setMinimumSize(10,10);
	setupSpacer1->setMaximumSize(9999,10);
	LabelWithToolTip *trainingsetTypeLabel = new LabelWithToolTip("  Trainingset Type");
	QWidget *trainingsetTypeWidget = new QWidget(configWidget);
	QGridLayout *trainingsettypelayout = new QGridLayout(trainingsetTypeWidget);
	trainingsettypelayout->setMargin(0);
	type3DButton = new QRadioButton("3D",this);
	type3DButton->setChecked(true);
	connect(type3DButton, &QRadioButton::toggled, this, &ExportTrainingsetWidget::trainingsetTypeChangedSlot);
	type2DButton = new QRadioButton("2D",this);
	trainingsettypelayout->addWidget(type3DButton,0,1);
	trainingsettypelayout->addWidget(type2DButton,0,0);
	LabelWithToolTip *intrinsicsPathLabel = new LabelWithToolTip("  Intrinsics Folder Path");
	intrinsicsPathWidget = new DirPathWidget("Select Intrinsics Path");
	intrinsicsPathWidget->setPlaceholderText("Select a Path...");
	LabelWithToolTip *extrinsicsPathLabel = new LabelWithToolTip("  Extrinsics Folder Path");
	extrinsicsPathWidget = new DirPathWidget("Select Extrinsics Path");
	extrinsicsPathWidget->setPlaceholderText("Select a Path...");
	QWidget *setupSpacer2 = new QWidget(configWidget);

	setupSpacer2->setMinimumSize(10,10);
	setupSpacer2->setMaximumSize(9999,10);
	LabelWithToolTip *validationFractionLabel = new LabelWithToolTip("  Validation Split Fraction");
	validationFractionEdit = new QDoubleSpinBox(configWidget);
	validationFractionEdit->setRange(0.001,0.999);
	validationFractionEdit->setValue(0.1);
	LabelWithToolTip *shuffleBeforeSplitLabel = new LabelWithToolTip("  Shuffle before Train/Validation Split");
	shuffleBeforeSplitWidget = new YesNoRadioWidget(configWidget);
	shuffleBeforeSplitWidget->setState(true);
	connect(shuffleBeforeSplitWidget, &YesNoRadioWidget::stateChanged, this, &ExportTrainingsetWidget::shuffleBeforeSplitStateChangedSlot);
	LabelWithToolTip *randomShuffleSeedLabel = new LabelWithToolTip("  Random Shuffle Seed");
	randomShuffleSeedWidget = new YesNoRadioWidget(configWidget);
	randomShuffleSeedWidget->setState(true);
	connect(randomShuffleSeedWidget, &YesNoRadioWidget::stateChanged, this, &ExportTrainingsetWidget::randomShuffleSeedStateChangedSlot);
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
	configurationlayout->addWidget(randomShuffleSeedLabel,i,0);
	configurationlayout->addWidget(randomShuffleSeedWidget,i++,1);
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


	compositionChartView = new CompositionChartView();
	compositionChartView->update();
	compositionChartView->setMinimumSize(375,200);
	compositionChartView->setMaximumSize(400,350);
	compositionChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(compositionChartView, &CompositionChartView::hoverStarted, this, &ExportTrainingsetWidget::hoverStartedSlot);
	connect(compositionChartView, &CompositionChartView::hoverEnded, this, &ExportTrainingsetWidget::hoverEndedSlot);

	annotationStatisticsBox = new QGroupBox(this);
	annotationStatisticsBox->setStyleSheet("QGroupBox{margin-top:0px; background-color:rgba(127,127,127,100)}");
	QGridLayout *annotationsstatisticslayout = new QGridLayout(annotationStatisticsBox);
	annotationStatisticsLabel = new QLabel("Trainingset Statistics");
	annotationStatisticsLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	LabelWithToolTip *numberFramesTrainingLabel = new LabelWithToolTip("  Frames in Training Split");
	numberFramesTrainingEdit = new QLineEdit(summaryWidget);
	numberFramesTrainingEdit->setReadOnly(true);
	numberFramesTrainingEdit->setText(QString::number(0));
	LabelWithToolTip *numberFramesValidationLabel = new LabelWithToolTip("  Frames in Validation Split");
	numberFramesValidationEdit = new QLineEdit(summaryWidget);
	numberFramesValidationEdit->setReadOnly(true);
	numberFramesValidationEdit->setText(QString::number(0));
	annotationChartView = new AnnotationStatisticsChartView();
	annotationChartView->setMaximumSize(900,250);
	annotationChartView->setMinimumSize(375,100);
	i = 0;
	annotationsstatisticslayout->addWidget(annotationStatisticsLabel,i++,0,1,2, Qt::AlignCenter);
	annotationsstatisticslayout->addWidget(numberFramesTrainingLabel,i,0);
	annotationsstatisticslayout->addWidget(numberFramesTrainingEdit,i++,1);
	annotationsstatisticslayout->addWidget(numberFramesValidationLabel,i,0);
	annotationsstatisticslayout->addWidget(numberFramesValidationEdit,i++,1);
	annotationsstatisticslayout->addWidget(annotationChartView,i++,0,1,2);

	summarylayout->addWidget(compositionChartView,0,0);
	summarylayout->addWidget(annotationStatisticsBox,0,1);



	QWidget *listWidget = new QWidget(this);
	QGridLayout *listlayout = new QGridLayout(listWidget);
	listlayout->setRowStretch(0,2);
	listlayout->setRowStretch(1,1);
	listlayout->setRowStretch(2,2);
	listlayout->setMargin(0);

	QGroupBox *datasetListBox = new QGroupBox("Datasets", this);
	QGridLayout *datasetlistlayout = new QGridLayout(datasetListBox);
	datasetlistlayout->setMargin(0);
	datasetList = new DatasetList(m_datasetExportItems, m_entities, m_keypoints, datasetListBox);
	connect(datasetList, &DatasetList::itemsChanged, this, &ExportTrainingsetWidget::datasetListChangedSlot);
	datasetlistlayout->addWidget(datasetList,0,0);


	QGroupBox *entitiesListBox = new QGroupBox("Entities", this);
	QGridLayout *enitieslistlayout = new QGridLayout(entitiesListBox);
	enitieslistlayout->setMargin(0);
	entitiesList = new CheckableItemList("Entities", entitiesListBox);
	connect(entitiesList, &CheckableItemList::checkStateChanged, this, &ExportTrainingsetWidget::entitiesListChangedSlot);
	enitieslistlayout->addWidget(entitiesList);

	QGroupBox *keypointListBox = new QGroupBox("Keypoints", this);
	QGridLayout *keypointlistlayout = new QGridLayout(keypointListBox);
	keypointlistlayout->setMargin(0);
	keypointsList = new CheckableItemList("Entities", keypointListBox);
	connect(keypointsList, &CheckableItemList::checkStateChanged, this, &ExportTrainingsetWidget::keypointsListChangedSlot);
	keypointlistlayout->addWidget(keypointsList);


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
	exportButton = new QPushButton("Create Trainingset");
	exportButton->setIcon(QIcon::fromTheme("start"));
	exportButton->setMinimumSize(40,40);
	connect(exportButton, &QPushButton::clicked, this, &ExportTrainingsetWidget::exportClickedSlot);
	buttonbarlayout->addWidget(saveButton, 0,0);
	buttonbarlayout->addWidget(loadButton,0,1);
	buttonbarlayout->addWidget(middleSpacer,0,2);
	buttonbarlayout->addWidget(exportButton,0,3);

	layout->addWidget(exportTrainingsSetLabel,0,0,1,2);
	layout->addWidget(configurationBox,1,0);
	layout->addWidget(summaryBox,2,0);
	layout->addWidget(listWidget,1,1,2,1);
	layout->addWidget(buttonBarWidget, 3,0,1,2);

	connect(this, &ExportTrainingsetWidget::updateCounts, datasetList, &DatasetList::updateCountsSlot);
	connect(this, &ExportTrainingsetWidget::exportTrainingset,trainingSetExporter, &TrainingSetExporter::exportTrainingsetSlot);

}


void ExportTrainingsetWidget::hoverStartedSlot(int index) {
	annotationStatisticsLabel->setText(m_datasetExportItems[index].name +  " Statistics");
	annotationChartView->update(&m_datasetExportItems, index);

	QList<QString> colors = {"rgba(51,34,136,70", "rgba(60,179,238,70", "rgba(43,170,149,70",
													 "rgba(30,119,60,70", "rgba(153,153,38,70", "rgba(221,193,55,70",
													 "rgba(163,24,27,70", "rgba(136,34,85,70",  "rgba(170,43,149,70"};
	QString styleSheet = "QGroupBox{margin-top:0px; background-color:%1);}";
	annotationStatisticsBox->setStyleSheet(styleSheet.arg(colors[index%colors.size()]));
	numberFramesTrainingEdit->setText(QString::number(static_cast<int>(m_datasetExportItems[index].frameCount*(1.0-validationFractionEdit->value()))));
	numberFramesValidationEdit->setText(QString::number(static_cast<int>(m_datasetExportItems[index].frameCount*validationFractionEdit->value())));
}


void ExportTrainingsetWidget::hoverEndedSlot() {
	annotationStatisticsLabel->setText("Trainingset Statistics");
	annotationChartView->update(&m_datasetExportItems, -1);
	annotationStatisticsBox->setStyleSheet("QGroupBox{margin-top:0px; background-color:rgba(127,127,127,100)}");

	int totalFrameCount = 0;
	for (const auto &exportItem : m_datasetExportItems) {
		totalFrameCount += exportItem.frameCount;
	}
	numberFramesTrainingEdit->setText(QString::number(static_cast<int>(totalFrameCount*(1.0-validationFractionEdit->value()))));
	numberFramesValidationEdit->setText(QString::number(static_cast<int>(totalFrameCount*validationFractionEdit->value())));
}


void ExportTrainingsetWidget::trainingsetTypeChangedSlot(bool toggle) {
	intrinsicsPathWidget->setEnabled(toggle);
	extrinsicsPathWidget->setEnabled(toggle);
}


void ExportTrainingsetWidget::shuffleBeforeSplitStateChangedSlot(bool state) {
	if (state == true) {
		randomShuffleSeedWidget->setEnabled(true);
		shuffleSeedEdit->setEnabled(!randomShuffleSeedWidget->state());
	}
	else {
		randomShuffleSeedWidget->setEnabled(false);
		shuffleSeedEdit->setEnabled(false);
	}
}


void ExportTrainingsetWidget::randomShuffleSeedStateChangedSlot(bool state) {
	shuffleSeedEdit->setEnabled(!state);
}


void ExportTrainingsetWidget::datasetListChangedSlot() {
	compositionChartView->update(&m_datasetExportItems);
	annotationChartView->update(&m_datasetExportItems, -1);
	int totalFrameCount = 0;
	for (const auto &exportItem : m_datasetExportItems) {
		totalFrameCount += exportItem.frameCount;
	}
	numberFramesTrainingEdit->setText(QString::number(static_cast<int>(totalFrameCount*(1.0-validationFractionEdit->value()))));
	numberFramesValidationEdit->setText(QString::number(static_cast<int>(totalFrameCount*validationFractionEdit->value())));
	entitiesList->clear();
	for (const auto &entity : m_entities) {
		entitiesList->addItem(entity.first, entity.second);
	}
	keypointsList->clear();
	for (const auto &keypoint : m_keypoints) {
		keypointsList->addItem(keypoint.first, keypoint.second);
	}
}


void ExportTrainingsetWidget::entitiesListChangedSlot(int row, bool state) {
	m_entities[row].second = state;
	emit updateCounts();
}


void ExportTrainingsetWidget::keypointsListChangedSlot(int row, bool state) {
	m_keypoints[row].second = state;
	emit updateCounts();
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

	settings->setValue("trainingSetName", trainingsetNameEdit->text());
	settings->setValue("trainingSetPath", trainingsetSavePathWidget->path());
	settings->setValue("trainingSetType", type3DButton->isChecked());
	settings->setValue("intrinsicsPath", intrinsicsPathWidget->path());
	settings->setValue("extrinsicsPath", extrinsicsPathWidget->path());
	settings->setValue("validationFraction", validationFractionEdit->value());
	settings->setValue("shuffleBeforeSplit", shuffleBeforeSplitWidget->state());
	settings->setValue("randomShuffleSeed", randomShuffleSeedWidget->state());
	settings->setValue("shuffleSeed", shuffleSeedEdit->value());

	settings->setValue("datasetList", QVariant::fromValue(datasetList->itemPaths()));

	QList<QList<QPair<QString, bool>>> subSetList;
	for (const auto& exportItem : m_datasetExportItems) {
		subSetList.append(exportItem.subSets);
	}
	settings->setValue("subSetList", QVariant::fromValue(subSetList));

	QMap<QString, bool> entitiesCheckMap;
	for (const auto &entity : m_entities) {
		entitiesCheckMap[entity.first] = entity.second;
	}
	settings->setValue("entitiesCheckMap", QVariant::fromValue(entitiesCheckMap));

	QMap<QString, bool> keypointsCheckMap;
	for (const auto &keypoint : m_keypoints) {
		keypointsCheckMap[keypoint.first] = keypoint.second;
	}
	settings->setValue("keypointsCheckMap", QVariant::fromValue(keypointsCheckMap));

	settings->endGroup();
}


void ExportTrainingsetWidget::loadPresetSlot(const QString& preset) {
	settings->beginGroup(preset);

	trainingsetNameEdit->setText(settings->value("trainingSetName").toString());
	trainingsetSavePathWidget->setPath(settings->value("trainingSetPath").toString());
	if (settings->value("trainingSetType").toBool()) {
		type3DButton->setChecked(true);
	}
	else {
		type2DButton->setChecked(true);
	}
	intrinsicsPathWidget->setPath(settings->value("intrinsicsPath").toString());
	extrinsicsPathWidget->setPath(settings->value("extrinsicsPath").toString());
	validationFractionEdit->setValue(settings->value("validationFraction").toDouble());
	shuffleBeforeSplitWidget->setState(settings->value("shuffleBeforeSplit").toBool());
	randomShuffleSeedWidget->setState(settings->value("randomShuffleSeed").toBool());
	shuffleSeedEdit->setValue(settings->value("shuffleSeed").toInt());


	QList<QString> datasetItemPaths = settings->value("datasetList").value<QList<QString>>();
	datasetList->clear();
	for (const auto &itemPath : datasetItemPaths) {
		datasetList->addItem(itemPath);
	}

	QList<QList<QPair<QString, bool>>> subSetList = settings->value("subSetList").value<QList<QList<QPair<QString, bool>>>>();
	int index = 0;
	for (auto& exportItem : m_datasetExportItems) {
		exportItem.subSets = subSetList[index++];
	}

	QMap<QString, bool> entitiesCheckMap = settings->value("entitiesCheckMap").value<QMap<QString, bool>>();
	for (auto & entity : m_entities) {
		if (entitiesCheckMap.contains(entity.first)) {
			entity.second = entitiesCheckMap[entity.first];
		}
	}
	entitiesList->clear();
	for (const auto & entity : m_entities) {
		entitiesList->addItem(entity.first, entity.second);
	}

	QMap<QString, bool> keypointsCheckMap = settings->value("keypointsCheckMap").value<QMap<QString, bool>>();
	for (auto & keypoint : m_keypoints) {
		if (keypointsCheckMap.contains(keypoint.first)) {
			keypoint.second = keypointsCheckMap[keypoint.first];
		}
	}
	keypointsList->clear();
	for (const auto & keypoint : m_keypoints) {
		keypointsList->addItem(keypoint.first, keypoint.second);
	}
	settings->endGroup();
	emit updateCounts();
}

void ExportTrainingsetWidget::exportClickedSlot() {
	ExportConfig exportConfig;
	if (m_datasetExportItems.size() == 0 || m_entities.size() == 0 || m_keypoints.size() == 0) {
		return;
	}
	exportConfig.entitiesList = m_entities;
	exportConfig.keypointsList = m_keypoints;
	if (trainingsetNameEdit->text() == "") {
		return;
	}
	exportConfig.trainingSetName = trainingsetNameEdit->text();
	if (trainingsetSavePathWidget->path() == "") {
		return;
	}
	exportConfig.savePath = trainingsetSavePathWidget->path();
	if (type3DButton->isChecked()) {
		exportConfig.trainingSetType = "3D";
		if (intrinsicsPathWidget->path() == "") {
			return;
		}
		exportConfig.intrinsicsPath = intrinsicsPathWidget->path();
		if (extrinsicsPathWidget->path() == "") {
			return;
		}
		exportConfig.extrinsicsPath = extrinsicsPathWidget->path();
	}
	else {
		exportConfig.trainingSetType = "2D";
	}
	exportConfig.validationFraction = validationFractionEdit->value();
	exportConfig.shuffleBeforeSplit = shuffleBeforeSplitWidget->state();
	if (shuffleBeforeSplitWidget->state()) {
		exportConfig.useRandomShuffleSeed = randomShuffleSeedWidget->state();
		if (!randomShuffleSeedWidget->state()) {
			exportConfig.shuffleSeed = shuffleSeedEdit->value();
		}
	}
	emit exportTrainingset(exportConfig);

}
