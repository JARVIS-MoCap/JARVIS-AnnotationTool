/*****************************************************************
	* File:			  settingswindow.cpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "settingswindow.hpp"

#include <QSpinBox>
#include <QColorDialog>


SettingsWindow::SettingsWindow(QWidget *parent) : QWidget(parent, Qt::Window) {
	this->resize(600,600);
	this->setMinimumSize(500,550);
	settings = new QSettings();
	setWindowTitle("Settings");
	QGridLayout *settingslayout = new QGridLayout(this);

	imageSettingsBox = new QGroupBox("Image Settings");
	QGridLayout *imagesettingslayout = new QGridLayout(imageSettingsBox);
	QLabel *grayScaleLabel = new QLabel("Display as Grayscale");
	grayScaleToggle = new QCheckBox();
	grayScaleToggle->setMinimumSize(30,30);
	connect(grayScaleToggle, &QCheckBox::stateChanged, this, &SettingsWindow::grayScaleToggledSlot);
	QLabel *hueLabel = new QLabel("Hue");
	hueSlider = new QSlider(Qt::Horizontal,this);
	hueSlider->setRange(0,359);
	hueSlider->setValue(0);
	hueBox = new QSpinBox(this);
	hueBox->setRange(0,359);
	hueBox->setValue(0);
	hueBox->setMinimumSize(80,30);
	hueResetButton = new QPushButton(this);
	hueResetButton->setMinimumSize(30,30);
	hueResetButton->setIcon(QIcon::fromTheme("reset"));
	connect(hueSlider, &QSlider::valueChanged, hueBox, &QSpinBox::setValue);
	connect(hueBox, QOverload<int>::of(&QSpinBox::valueChanged), hueSlider, &QSlider::setValue);
	connect(hueBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::imageTranformationChangedSlot);
	connect(hueResetButton, &QPushButton::clicked, this, &SettingsWindow::hueResetClickedSlot);
	QLabel *saturationLabel = new QLabel("Saturation");
	saturationSlider = new QSlider(Qt::Horizontal,this);
	saturationSlider->setRange(0,200);
	saturationSlider->setValue(100);
	saturationBox = new QSpinBox(this);
	saturationBox->setRange(0,200);
	saturationBox->setValue(100);
	saturationBox->setMinimumSize(80,30);
	saturationResetButton = new QPushButton(this);
	saturationResetButton->setMinimumSize(30,30);
	saturationResetButton->setIcon(QIcon::fromTheme("reset"));
	connect(saturationSlider, &QSlider::valueChanged, saturationBox, &QSpinBox::setValue);
	connect(saturationBox, QOverload<int>::of(&QSpinBox::valueChanged), saturationSlider, &QSlider::setValue);
	connect(saturationBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::imageTranformationChangedSlot);
	connect(saturationResetButton, &QPushButton::clicked, this, &SettingsWindow::saturationResetClickedSlot);
	QLabel *brightnessLabel = new QLabel("Brightness");
	brightnessSlider = new QSlider(Qt::Horizontal,this);
	brightnessSlider->setRange(0,200);
	brightnessSlider->setValue(100);
	brightnessBox = new QSpinBox(this);
	brightnessBox->setRange(0,200);
	brightnessBox->setValue(100);
	brightnessBox->setMinimumSize(80,30);
	brightnessResetButton = new QPushButton(this);
	brightnessResetButton->setMinimumSize(30,30);
	brightnessResetButton->setIcon(QIcon::fromTheme("reset"));
	connect(brightnessSlider, &QSlider::valueChanged, brightnessBox, &QSpinBox::setValue);
	connect(brightnessBox, QOverload<int>::of(&QSpinBox::valueChanged), brightnessSlider, &QSlider::setValue);
	connect(brightnessBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::imageTranformationChangedSlot);
	connect(brightnessResetButton, &QPushButton::clicked, this, &SettingsWindow::brightnessResetClickedSlot);

	imagesettingslayout->addWidget(grayScaleLabel, 0,0);
	imagesettingslayout->addWidget(grayScaleToggle,0,1,1,3);
	imagesettingslayout->addWidget(hueLabel,1,0);
	imagesettingslayout->addWidget(hueSlider,1,1);
	imagesettingslayout->addWidget(hueBox,1,2);
	imagesettingslayout->addWidget(hueResetButton,1,3);
	imagesettingslayout->addWidget(saturationLabel,2,0);
	imagesettingslayout->addWidget(saturationSlider,2,1);
	imagesettingslayout->addWidget(saturationBox,2,2);
	imagesettingslayout->addWidget(saturationResetButton,2,3);
	imagesettingslayout->addWidget(brightnessLabel,3,0);
	imagesettingslayout->addWidget(brightnessSlider,3,1);
	imagesettingslayout->addWidget(brightnessBox,3,2);
	imagesettingslayout->addWidget(brightnessResetButton,3,3);


	annotationSettingsBox = new QGroupBox("Keypoint Label Settings");
	QGridLayout *annotationsettingslayout = new QGridLayout(annotationSettingsBox);
	QLabel *keypointSizeLabel = new QLabel("Keypoint Label Size");
	keypointSizeEdit = new QSpinBox(annotationSettingsBox);
	keypointSizeEdit->setRange(0,100);
	keypointSizeEdit->setValue(8);
	connect(keypointSizeEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::keypointSizeChanged);
	entitySettingsWidget = new QWidget(annotationSettingsBox);
	entitysettingslayout = new QGridLayout(entitySettingsWidget);
	annotationsettingslayout->addWidget(keypointSizeLabel,0,0);
	annotationsettingslayout->addWidget(keypointSizeEdit,0,1);
	annotationsettingslayout->addWidget(entitySettingsWidget,1,0,1,2);

	reprojectionSettingsBox = new QGroupBox("Reprojection Settings");
	QGridLayout *reprojectionsettingslayout = new QGridLayout(reprojectionSettingsBox);
	QLabel *minViewsLabel = new QLabel("Min Annotated Views");
	minViewsEdit = new QSpinBox(reprojectionSettingsBox);
	minViewsEdit->setRange(2,12);
	minViewsEdit->setValue(2);
	connect(minViewsEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::minViewsChangedSlot);
	QLabel *errorThresholdLabel = new QLabel("Repro Error Threshold");
	errorThresholdEdit = new QDoubleSpinBox();
	errorThresholdEdit->setValue(10.0);
	connect(errorThresholdEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SettingsWindow::errorThresholdChangedSlot);
	reprojectionsettingslayout->addWidget(minViewsLabel,0,0);
	reprojectionsettingslayout->addWidget(minViewsEdit,0,1);
	reprojectionsettingslayout->addWidget(errorThresholdLabel,1,0);
	reprojectionsettingslayout->addWidget(errorThresholdEdit,1,1);


	settingslayout->addWidget(imageSettingsBox,0,0);
	settingslayout->addWidget(annotationSettingsBox,1,0);
	settingslayout->addWidget(reprojectionSettingsBox,2,0);

	loadSettings();
}


void SettingsWindow::datasetLoadedSlot() {
	int i = 0;
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		QWidget *singleEntityWidget = new QWidget(entitySettingsWidget);
		QLabel *entityLabel = new QLabel(entity);
		entityLabel->setFont(QFont("Sans Serif", 11, QFont::Bold));
		QGridLayout *singleentitylayout = new QGridLayout(singleEntityWidget);
		singleentitylayout->setMargin(0);
		QLabel *keypointShapeLabel = new QLabel(" Keypoint Shape");
		QComboBox * keypointShapeCombo = new QComboBox();
		keypointShapeCombo->addItem("Circle");
		keypointShapeCombo->addItem("Rectangle");
		connect(keypointShapeCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
						this, &SettingsWindow::keypointShapeChangedSlot);
		keypointShapeCombosList.append(keypointShapeCombo);
		QLabel *colorMapLabel = new QLabel(" Colormap"); //Make preview thingy here??
		QComboBox *colorMapCombo = new QComboBox();
		colorMapCombosList.append(colorMapCombo);
		colorMapTypeList.append(ColorMap::Jet);
		colorMapCombo->addItem("Jet");
		colorMapCombo->addItem("Fixed");
		colorMapCombo->addItem("Single");
		connect(colorMapCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
						this, &SettingsWindow::colorMapChangedSlot);
		QLabel *colormapPreviewLabel = new QLabel();
		colormapPreviewsList.append(colormapPreviewLabel);
		colormapPreviewLabel->setMinimumSize(200,20);
		QImage colormapPreview = createColorMapPreview(ColorMap::Jet);
		colormapPreviewLabel->setPixmap(QPixmap::fromImage(colormapPreview).scaled(200,20));
		QPushButton *colorChooserButton = new QPushButton();
		colorChooserButton->setMinimumSize(30,30);
		colorChooserButton->setMaximumSize(30,30);
		colorChooserButton->setIcon(QIcon::fromTheme("paint"));
		colorChooserButton->setEnabled(false);
		connect(colorChooserButton, &QPushButton::clicked, this, &SettingsWindow::colorChooserClickedSlot);
		colorChooserButtonsList.append(colorChooserButton);
		colorsList.append(QColor(0,0,255));
		singleentitylayout->addWidget(entityLabel,0,0,1,3);
		singleentitylayout->addWidget(keypointShapeLabel, 1,0);
		singleentitylayout->addWidget(keypointShapeCombo,1,1,1,2);
		singleentitylayout->addWidget(colorMapLabel,2,0);
		singleentitylayout->addWidget(colorMapCombo,2,1,1,2);
		singleentitylayout->addWidget(colormapPreviewLabel,3,1);
		singleentitylayout->addWidget(colorChooserButton,3,2);

		entitysettingslayout->addWidget(singleEntityWidget,i++,0);
	}
}


void SettingsWindow::loadSettings() {
	settings->beginGroup("Settings");
	settings->beginGroup("ReprojectionSettings");
	int minViews = 2;
	if (settings->contains("MinViews")) {
		minViews = settings->value("MinViews").toInt();
	}
	minViewsEdit->setValue(minViews);
	minViewsChangedSlot(minViews);
	double errorThreshold = 10.0;
	if (settings->contains("errorThreshold")) {
		errorThreshold = settings->value("errorThreshold").toDouble();
	}
	errorThresholdEdit->setValue(errorThreshold);
	errorThresholdChangedSlot(errorThreshold);
	settings->endGroup();
	settings->endGroup();
}


void SettingsWindow::imageTranformationChangedSlot() {
	emit imageTranformationChanged(hueBox->value(), saturationBox->value(), brightnessBox->value());
}


void SettingsWindow::grayScaleToggledSlot(int state) {
	if (state == Qt::Checked) {
		saturationSlider->setValue(0);
		saturationSlider->setEnabled(false);
		saturationBox->setEnabled(false);
		saturationResetButton->setEnabled(false);
		hueSlider->setValue(0);
		hueSlider->setEnabled(false);
		hueBox->setEnabled(false);
		hueResetButton->setEnabled(false);
	}
	else {
		saturationSlider->setValue(100);
		saturationSlider->setEnabled(true);
		saturationBox->setEnabled(true);
		saturationResetButton->setEnabled(true);
		hueSlider->setEnabled(true);
		hueBox->setEnabled(true);
		hueResetButton->setEnabled(true);
	}
}


void SettingsWindow::hueResetClickedSlot() {
	hueSlider->setValue(0);
}


void SettingsWindow::saturationResetClickedSlot() {
	saturationSlider->setValue(100);
}


void SettingsWindow::brightnessResetClickedSlot() {
	brightnessSlider->setValue(100);
}


void SettingsWindow::keypointShapeChangedSlot(int index) {
	int entityIndex = keypointShapeCombosList.indexOf(qobject_cast<QComboBox*>(sender()));
	emit keypointShapeChanged(Dataset::dataset->entitiesList()[entityIndex], static_cast<KeypointShape>(index));
}


QImage SettingsWindow::createColorMapPreview(ColorMap::ColorMapType type, QColor color) {
	QImage colormapPreview(Dataset::dataset->bodypartsList().size(), 1, QImage::Format_RGB888);
	for (int i = 0; i < Dataset::dataset->bodypartsList().size(); i++) {
		ColorMap *colorMap = new ColorMap(type, color);
		colormapPreview.setPixelColor(i,0,colorMap->getColor(i, Dataset::dataset->bodypartsList().size()));
	}
	return colormapPreview;
}


void SettingsWindow::colorMapChangedSlot(int index) {
	int entityIndex = colorMapCombosList.indexOf(qobject_cast<QComboBox*>(sender()));
	colorMapTypeList[entityIndex] = static_cast<ColorMap::ColorMapType>(index);
	colorChooserButtonsList[entityIndex]->setEnabled(index != 0);
	QImage colormapPreview = createColorMapPreview(colorMapTypeList[entityIndex], colorsList[entityIndex]);
	colormapPreviewsList[entityIndex]->setPixmap(QPixmap::fromImage(colormapPreview).scaled(200,20));
	emit colorMapChanged(Dataset::dataset->entitiesList()[entityIndex],
											 colorMapTypeList[entityIndex], colorsList[entityIndex]);
}


void SettingsWindow::colorChooserClickedSlot() {
	int entityIndex = colorChooserButtonsList.indexOf(qobject_cast<QPushButton*>(sender()));
	colorsList[entityIndex] = QColorDialog::getColor(colorsList[entityIndex], this);
	QImage colormapPreview = createColorMapPreview(colorMapTypeList[entityIndex], colorsList[entityIndex]);
	colormapPreviewsList[entityIndex]->setPixmap(QPixmap::fromImage(colormapPreview).scaled(200,20));
	emit colorMapChanged(Dataset::dataset->entitiesList()[entityIndex],
											 colorMapTypeList[entityIndex], colorsList[entityIndex]);
}


void SettingsWindow::minViewsChangedSlot(int val) {
	settings->beginGroup("Settings");
	settings->beginGroup("ReprojectionSettings");
	settings->setValue("MinViews", val);
	settings->endGroup();
	settings->endGroup();
	emit minViewsChanged(val);
}


void SettingsWindow::errorThresholdChangedSlot(double val) {
	settings->beginGroup("Settings");
	settings->beginGroup("ReprojectionSettings");
	settings->setValue("errorThreshold", val);
	settings->endGroup();
	settings->endGroup();
	emit errorThresholdChanged(val);
}
