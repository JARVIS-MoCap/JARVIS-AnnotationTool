/*****************************************************************
 * File:			  editowidget.cpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:  2022 Timo Hueser
 * License:    LGPL v2.1
 *****************************************************************/

#include "editorwidget.hpp"


EditorWidget::EditorWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	mainSplitter = new QSplitter(Qt::Vertical,this);
	connect(mainSplitter, &QSplitter::splitterMoved, this, &EditorWidget::splitterMovedSlot);

	horizontalSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
	connect(horizontalSplitter, &QSplitter::splitterMoved, this, &EditorWidget::splitterMovedSlot);

	leftSplitter = new QSplitter(Qt::Vertical,horizontalSplitter);
	leftSplitter->hide();

	visualizationWindow = new VisualizationWindow();
	//visualizationWindow->show();

	reprojectionWidget = new ReprojectionWidget(this);
	datasetControlWidget = new DatasetControlWidget(this);
	leftSplitter->addWidget(datasetControlWidget);
	leftSplitter->addWidget(reprojectionWidget);
	leftSplitter->setCollapsible(0,true);
	leftSplitter->setCollapsible(1,true);

	imageViewerContainer = new QWidget(this);
	imageViewerContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QGridLayout *containerlayout = new QGridLayout(imageViewerContainer);
	containerlayout->setContentsMargins(0,0,0,0);
	imageViewer = new ImageViewer();
	imageViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	containerlayout->addWidget(imageViewer,0,0);
	//QImage img = QImage("../frame_25400.jpg");
	//imageViewer->setImage(img);

	keypointWidget = new KeypointWidget(horizontalSplitter);
	keypointWidget->hide();

	buttonWidget = new QWidget(mainSplitter);
	buttonWidget->installEventFilter(this);
	QGridLayout *buttonlayout = new QGridLayout(buttonWidget);
	buttonlayout->setSpacing(20);
	previousButton = new QPushButton("<< Previous", buttonWidget);
	previousButton->installEventFilter(this);
	previousButton->setMinimumSize(130, 35);
	previousButton->setMaximumSize(130,35);
	previousButton->setEnabled(false);
	connect(previousButton, &QPushButton::clicked, this, &EditorWidget::previousClickedSlot);
	nextButton = new QPushButton("Next >>", buttonWidget);
	nextButton->installEventFilter(this);
	nextButton->setMinimumSize(130,35);
	nextButton->setMaximumSize(130,35);
	nextButton->setEnabled(false);
	connect(nextButton, &QPushButton::clicked, this, &EditorWidget::nextClickedSlot);
	QWidget *buttonSpacer1 = new QWidget(this);
	buttonSpacer1->setMaximumSize(100,100);
	buttonSpacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	cropButton = new QPushButton("Crop", buttonWidget);
	cropButton->installEventFilter(this);
	cropButton->setIcon(QIcon::fromTheme("search"));
	cropButton->setCheckable(true);
	cropButton->setMinimumSize(130,35);
	cropButton->setMaximumSize(130,35);
	connect(cropButton, &QPushButton::toggled, this, &EditorWidget::cropToggled);
	connect(cropButton, &QPushButton::toggled, this, &EditorWidget::cropToggledSlot);
	panButton = new QPushButton("Pan", buttonWidget);
	panButton->installEventFilter(this);
	panButton->setIcon(QIcon::fromTheme("move"));
	panButton->setCheckable(true);
	panButton->setMinimumSize(130,35);
	panButton->setMaximumSize(130,35);
	connect(panButton, &QPushButton::toggled, this, &EditorWidget::panToggled);
	connect(panButton, &QPushButton::toggled, this, &EditorWidget::panToggledSlot);
	homeButton = new QPushButton("Home", buttonWidget);
	homeButton->installEventFilter(this);
	homeButton->setIcon(QIcon::fromTheme("home"));
	homeButton->setMinimumSize(130,35);
	homeButton->setMaximumSize(130,35);
	connect(homeButton, &QPushButton::clicked, this, &EditorWidget::homeClicked);
	connect(homeButton, &QPushButton::clicked, this, &EditorWidget::homeClickedSlot);
	QWidget *buttonSpacer2 = new QWidget(this);
	buttonSpacer2->setMaximumSize(100,100);
	buttonSpacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	previousSetButton = new QPushButton("<< Previous Set", buttonWidget);
	previousSetButton->installEventFilter(this);
	previousSetButton->setMinimumSize(160,35);
	previousSetButton->setMaximumSize(160,35);
	previousSetButton->setEnabled(false);
	connect(previousSetButton, &QPushButton::clicked, this, &EditorWidget::previousSetClickedSlot);
	nextSetButton = new QPushButton("Next Set >>", buttonWidget);
	nextSetButton->installEventFilter(this);
	nextSetButton->setMinimumSize(160,35);
	nextSetButton->setMaximumSize(160,35);
	nextSetButton->setEnabled(false);
	connect(nextSetButton, &QPushButton::clicked, this, &EditorWidget::nextSetClickedSlot);
	QWidget *buttonSpacer3 = new QWidget(this);
	buttonSpacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	saveSetupButton = new QPushButton("Save Setup", buttonWidget);
	connect(saveSetupButton, &QPushButton::clicked, visualizationWindow, &VisualizationWindow::saveClickedSlot);
	saveSetupButton->setIcon(QIcon::fromTheme("upload"));
	saveSetupButton->installEventFilter(this);
	saveSetupButton->setMinimumSize(130,35);
	saveSetupButton->setMaximumSize(130,35);
	saveSetupButton->hide();
	show3DButton = new QPushButton("Viewer", buttonWidget);
	show3DButton->setIcon(QIcon::fromTheme("3d"));
	show3DButton->installEventFilter(this);
	show3DButton->setMinimumSize(130,35);
	show3DButton->setMaximumSize(130,35);
	connect(show3DButton, &QPushButton::clicked, this, &EditorWidget::show3DClickedSlot);
	buttonlayout->addWidget(previousButton,0,0);
	buttonlayout->addWidget(nextButton,0,1);
	buttonlayout->addWidget(buttonSpacer1,0,2);
	buttonlayout->addWidget(cropButton,0,3);
	buttonlayout->addWidget(panButton,0,4);
	buttonlayout->addWidget(homeButton,0,5);
	buttonlayout->addWidget(buttonSpacer2,0,6);
	buttonlayout->addWidget(previousSetButton,0,7);
	buttonlayout->addWidget(nextSetButton,0,8);
	buttonlayout->addWidget(buttonSpacer3,0,9);
	buttonlayout->addWidget(saveSetupButton,0,10);
	buttonlayout->addWidget(show3DButton,0,11);

	horizontalSplitter->addWidget(leftSplitter);
	horizontalSplitter->addWidget(imageViewerContainer);
	horizontalSplitter->addWidget(keypointWidget);
	horizontalSplitter->setSizes({200,1000,200});

	mainSplitter->addWidget(horizontalSplitter);
	mainSplitter->addWidget(buttonWidget);
	mainSplitter->setCollapsible(0,false);
	mainSplitter->setCollapsible(1,false);
	mainSplitter->setSizes({1000,50});

	layout->addWidget(mainSplitter,0,0);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals
	connect(imageViewer, &ImageViewer::zoomFinished, this, &EditorWidget::zoomFinishedSlot);
	connect(imageViewer, &ImageViewer::panFinished, this, &EditorWidget::panFinishedSlot);
	connect(datasetControlWidget, &DatasetControlWidget::frameSelectionChanged, this, &EditorWidget::frameChangedSlot);
	connect(datasetControlWidget, &DatasetControlWidget::imgSetChanged, this, &EditorWidget::imgSetChangedSlot);
	connect(datasetControlWidget, &DatasetControlWidget::datasetLoaded, this, &EditorWidget::datasetLoadedSlot);
	connect(imageViewer, &ImageViewer::brightnessChanged, this, &EditorWidget::brightnessChanged);


	//<- Outgoing Signals
	connect(this, &EditorWidget::cropToggled, imageViewer, &ImageViewer::cropToggledSlot);
	connect(this, &EditorWidget::panToggled, imageViewer, &ImageViewer::panToggledSlot);
	connect(this, &EditorWidget::homeClicked, imageViewer, &ImageViewer::homeClickedSlot);
	connect(this, &EditorWidget::imageTranformationChanged, imageViewer, &ImageViewer::imageTransformationChangedSlot);
	connect(this, &EditorWidget::alwaysShowLabelsToggled, imageViewer, &ImageViewer::alwaysShowLabelsToggledSlot);
	connect(this, &EditorWidget::labelFontColorChanged, imageViewer, &ImageViewer::labelFontColorChangedSlot);
	connect(this, &EditorWidget::labelBackgroundColorChanged, imageViewer, &ImageViewer::labelBackgroundColorChangedSlot);
	connect(this, &EditorWidget::keypointSizeChanged, imageViewer, &ImageViewer::keypointSizeChangedSlot);
	connect(this, &EditorWidget::keypointShapeChanged, imageViewer, &ImageViewer::keypointShapeChangedSlot);
	connect(this, &EditorWidget::colorMapChanged, imageViewer, &ImageViewer::colorMapChangedSlot);
	connect(this, &EditorWidget::frameChanged, keypointWidget, &KeypointWidget::frameChangedSlot);
	connect(this, &EditorWidget::frameChanged, reprojectionWidget, &ReprojectionWidget::calculateReprojectionSlot);
	connect(this, &EditorWidget::frameChanged, datasetControlWidget, &DatasetControlWidget::frameChangedSlot);
	connect(this, &EditorWidget::cmdRPressed, keypointWidget, &KeypointWidget::toggleCurrentKeypointSlot);
	connect(this, &EditorWidget::cmdEPressed, keypointWidget, &KeypointWidget::advanceCurrentKeypointSlot);

	//<-> Relayed Signals
	connect(datasetControlWidget, &DatasetControlWidget::datasetLoaded, this, &EditorWidget::newSegmentLoaded);
	connect(keypointWidget, &KeypointWidget::currentEntityChanged, imageViewer, &ImageViewer::currentEntityChangedSlot);
	connect(keypointWidget, &KeypointWidget::currentBodypartChanged, imageViewer, &ImageViewer::currentBodypartChangedSlot);
	connect(keypointWidget, &KeypointWidget::toggleEntityVisible, imageViewer, &ImageViewer::toggleEntityVisibleSlot);
	connect(keypointWidget, &KeypointWidget::updateViewer, imageViewer, &ImageViewer::updateViewer);
	connect(imageViewer, &ImageViewer::keypointAdded, keypointWidget, &KeypointWidget::keypointAddedSlot);
	connect(imageViewer, &ImageViewer::keypointRemoved, keypointWidget, &KeypointWidget::keypointRemovedSlot);
	connect(imageViewer, &ImageViewer::keypointCorrected, keypointWidget, &KeypointWidget::keypointCorrectedSlot);
	connect(imageViewer, &ImageViewer::alreadyAnnotated, keypointWidget, &KeypointWidget::alreadyAnnotatedSlot);
	connect(imageViewer, &ImageViewer::keypointChangedForReprojection, reprojectionWidget, &ReprojectionWidget::calculateReprojectionSlot);
	connect(reprojectionWidget, &ReprojectionWidget::reprojectedPoints, keypointWidget, &KeypointWidget::setKeypointsFromDatasetSlot);
	connect(reprojectionWidget, &ReprojectionWidget::reprojectionToolToggled, imageViewer, &ImageViewer::toggleReprojectionSlot);
	connect(reprojectionWidget, &ReprojectionWidget::update3DCoords, visualizationWindow, &VisualizationWindow::update3DCoordsSlot);
	connect(reprojectionWidget, &ReprojectionWidget::reprojectionToolUpdated, visualizationWindow, &VisualizationWindow::reprojectionToolUpdatedSlot);
	connect(this, &EditorWidget::minViewsChanged, reprojectionWidget, &ReprojectionWidget::minViewsChangedSlot);
	connect(this, &EditorWidget::errorThresholdChanged, reprojectionWidget, &ReprojectionWidget::errorThresholdChanged);
	connect(this, &EditorWidget::boneLengthErrorThresholdChanged, reprojectionWidget, &ReprojectionWidget::boneLengthErrorThresholdChanged);
}


void EditorWidget::splitterMovedSlot(int, int) {
	int maxWidth = mainSplitter->size().width();
	int maxHeight = mainSplitter->size().height();
	int width, height;
	float aspectRatio = 1280.0/1024.0;
	if ((maxHeight)*aspectRatio > maxWidth) {
		width = maxWidth;
		height = width/aspectRatio;
	}
	else {
		height = maxHeight;
		width =	(height)*aspectRatio;
	}
	imageViewer->setMaximumSize(width,height);
	imageViewer->setSize(width,height);

	imageViewer->fitToScreen();
}


void EditorWidget::previousClickedSlot() {
	if(m_currentFrameIndex > 0) {
		m_currentFrameIndex--;
		imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
		if (m_currentFrameIndex == 0) {
			previousButton->setEnabled(false);
		}
		if (m_currentFrameIndex == m_currentImgSet->numCameras - 2) {
			nextButton->setEnabled(true);
		}
	}
	emit frameChanged(m_currentImgSetIndex, m_currentFrameIndex);
}


void EditorWidget::nextClickedSlot() {
	if(m_currentFrameIndex < m_currentImgSet->numCameras-1) {
		m_currentFrameIndex++;
		imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
		if (m_currentFrameIndex == m_currentImgSet->numCameras-1) {
			nextButton->setEnabled(false);
		}
		if (m_currentFrameIndex == 1) {
			previousButton->setEnabled(true);
		}
	}
	emit frameChanged(m_currentImgSetIndex, m_currentFrameIndex);
}


void EditorWidget::frameChangedSlot(int index) {
	m_currentFrameIndex = index;
	imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
	if (m_currentFrameIndex == m_currentImgSet->numCameras-1) {
		nextButton->setEnabled(false);
		if (m_currentImgSet->numCameras > 1) {
			previousButton->setEnabled(true);
		}
	}
	else if (m_currentFrameIndex == 0) {
		previousButton->setEnabled(false);
		if (m_currentImgSet->numCameras > 1) {
			nextButton->setEnabled(true);
		}
	}
	else {
		previousButton->setEnabled(true);
		nextButton->setEnabled(true);
	}
	emit frameChanged(m_currentImgSetIndex, m_currentFrameIndex);
}


void EditorWidget::previousSetClickedSlot() {
	if (m_currentImgSetIndex > 0) {
		m_currentImgSetIndex--;
		m_currentFrameIndex = 0;
		previousButton->setEnabled(false);
		if (m_currentImgSet->numCameras > 1) {
			nextButton->setEnabled(true);
		}
		m_currentImgSet = Dataset::dataset->imgSets()[m_currentImgSetIndex];
		imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
		if (m_currentImgSetIndex == 0) {
			previousSetButton->setEnabled(false);
			if (Dataset::dataset->imgSets().size() > 1) {
				nextSetButton->setEnabled(true);
			}
		}
		else if (m_currentImgSetIndex == Dataset::dataset->imgSets().size()-2) {
			nextSetButton->setEnabled(true);
			if (Dataset::dataset->imgSets().size() > 1) {
				previousSetButton->setEnabled(true);
			}
		}
	}
	emit frameChanged(m_currentImgSetIndex, m_currentFrameIndex);
}


void EditorWidget::nextSetClickedSlot() {
	if (m_currentImgSetIndex < Dataset::dataset->imgSets().size()-1) {
		m_currentImgSetIndex++;
		m_currentFrameIndex = 0;
		previousButton->setEnabled(false);
		if (m_currentImgSet->numCameras > 1) {
			nextButton->setEnabled(true);
		}
		m_currentImgSet = Dataset::dataset->imgSets()[m_currentImgSetIndex];
		imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
		if (m_currentImgSetIndex == Dataset::dataset->imgSets().size()-1) {
			nextSetButton->setEnabled(false);
			if (Dataset::dataset->imgSets().size() > 1) {
				previousSetButton->setEnabled(true);
			}
		}
		else if (m_currentImgSetIndex == 1) {
			previousSetButton->setEnabled(true);
			if (Dataset::dataset->imgSets().size() > 1) {
				nextSetButton->setEnabled(true);
			}
		}
	}
	emit frameChanged(m_currentImgSetIndex, m_currentFrameIndex);
}


void EditorWidget::imgSetChangedSlot(int index) {
	m_currentImgSetIndex = index;
	m_currentImgSet = Dataset::dataset->imgSets()[m_currentImgSetIndex];
	imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
	if (m_currentImgSetIndex == Dataset::dataset->imgSets().size()-1) {
		nextSetButton->setEnabled(false);
		previousSetButton->setEnabled(true);
	}
	else if (m_currentImgSetIndex == 0) {
		previousSetButton->setEnabled(false);
		nextSetButton->setEnabled(true);
	}
	else {
		previousSetButton->setEnabled(true);
		nextSetButton->setEnabled(true);
	}
	emit frameChanged(m_currentImgSetIndex, m_currentFrameIndex);
}


void EditorWidget::cropToggledSlot(bool toggle) {
	if (toggle) panButton->setChecked(false);
}


void EditorWidget::panToggledSlot(bool toggle) {
	if (toggle) cropButton->setChecked(false);
}


void EditorWidget::homeClickedSlot() {
	cropButton->setChecked(false);
	panButton->setChecked(false);
}


void EditorWidget::zoomFinishedSlot() {
	cropButton->setChecked(false);
}


void EditorWidget::panFinishedSlot() {
	panButton->setChecked(false);
}


void EditorWidget::datasetLoadedSlot(bool isSetupAnnotation, QString selectedSegment) {
	if (isSetupAnnotation) {
		saveSetupButton->show();
	}
	else {
		saveSetupButton->hide();
	}
	keypointWidget->init();
	leftSplitter->show();
	leftSplitter->setSizes({500,800});
	keypointWidget->show();
	m_currentImgSet = Dataset::dataset->imgSets()[0];
	m_currentImgSetIndex = 0;
	m_currentFrameIndex = 0;
	imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);

	if (m_currentImgSet->numCameras != 1)
	{
		nextButton->setEnabled(true);
	}
	if (Dataset::dataset->imgSets().size() > 1)
	{
		nextSetButton->setEnabled(true);
	}
	previousButton->setEnabled(false);
	previousSetButton->setEnabled(false);
	splitterMovedSlot(0,0);
	keypointWidget->datasetLoadedSlot();
	reprojectionWidget->datasetLoadedSlot();
	datasetControlWidget->datasetLoadedSlot(selectedSegment);
	connect(Dataset::dataset, &Dataset::keypointStateChanged, datasetControlWidget, &DatasetControlWidget::keypointStateChangedSlot);
	emit datasetLoaded(selectedSegment);
}

void EditorWidget::show3DClickedSlot() {
	visualizationWindow->show();
	visualizationWindow->raise();
	visualizationWindow->opened();
}

void EditorWidget::keyPressEvent(QKeyEvent *e)
{
	int key = e->key();
	if (key == 16777236 || key == 68)
	{
		nextClickedSlot();
	}
	else if (key == 16777234 || key == 65) {
		previousClickedSlot();
	}
	if (key == 16777237 || key == 83)
	{
		if (e->modifiers() & Qt::ControlModifier)
		{
			int brightness = imageViewer->getBrightnessFactor();
			if (brightness > 0)
			{
				brightness = std::max(brightness - 4, 0);
			}
			imageViewer->setBrightness(brightness);
		}
		else
		{
			nextSetClickedSlot();
		}
	}
	else if (key == 16777235 || key == 87)
	{
		if (e->modifiers() & Qt::ControlModifier)
		{
			int brightness = imageViewer->getBrightnessFactor();
			if (brightness < 200)
			{
				brightness = std::min(brightness + 4, 200);
			}
			imageViewer->setBrightness(brightness);
		}
		else
		{
			previousSetClickedSlot();
		}
	}
	else if (key == 72) {
		emit homeClicked();
		homeClickedSlot();
	}
	else if (key == 67) {
		bool toggled = cropButton->isChecked();
		cropButton->setChecked(!toggled);
		emit cropToggled(!toggled);
		cropToggledSlot(!toggled);
        } 
	else if (key == 16777248) {
			keypointWidget->toggleHideAll();
		}
	else if ((key == Qt::Key_R) && (e->modifiers() & Qt::ControlModifier)){
		emit cmdRPressed();  // will suppress currently selected keypoint
	}
	else if ((key == Qt::Key_E) && (e->modifiers() & Qt::ControlModifier)){
		emit cmdEPressed();  // will skip currently selected keypoint
	}
}