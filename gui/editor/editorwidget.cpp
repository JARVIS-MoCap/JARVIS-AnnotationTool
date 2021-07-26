/*------------------------------------------------------------
 *  editorwidget.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "editorwidget.hpp"

EditorWidget::EditorWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	mainSplitter = new QSplitter(Qt::Vertical,this);
	connect(mainSplitter, &QSplitter::splitterMoved, this, &EditorWidget::splitterMovedSlot);

	horizontalSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
	connect(horizontalSplitter, &QSplitter::splitterMoved, this, &EditorWidget::splitterMovedSlot);

	leftSplitter = new QSplitter(Qt::Vertical,horizontalSplitter);
	leftSplitter->hide();

	reprojectionWidget = new ReprojectionWidget(this);
	datasetControlWidget = new DatasetControlWidget(this);
	leftSplitter->addWidget(datasetControlWidget);
	leftSplitter->addWidget(reprojectionWidget);
	leftSplitter->setCollapsible(0,true);
	leftSplitter->setCollapsible(1,true);


	stackedWidget = new QStackedWidget(this);

	datasetWidget = new QWidget(this);
	QSpacerItem *datasetTopSpacer = new QSpacerItem(100,300,QSizePolicy::Maximum,QSizePolicy::Maximum);
	QSpacerItem *datasetBottomSpacer = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
	QGridLayout *datasetlayout = new QGridLayout(datasetWidget);
	datasetlayout->setSpacing(16);
	QLabel *datasetLabel = new QLabel("Load existing Dataset or create new one?");
	datasetLabel->setFont(QFont("Sans Serif", 22, QFont::Bold));
	datasetLabel->setStyleSheet("QLabel {color: rgb(32,100,164)}");
	loadDatasetButton = new QPushButton("Load Dataset");
	loadDatasetButton->setMinimumSize(450,80);
	loadDatasetButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	loadDatasetButton->setStyleSheet(" QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
													 "QPushButton:hover { background-color: rgb(68,74,89); }"
													 "QPushButton { color: rgb(100,164,32);}");
	newDatasetButton = new QPushButton("New Dataset");
	newDatasetButton->setMinimumSize(450,80);
	newDatasetButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	newDatasetButton->setStyleSheet(" QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
													 "QPushButton:hover { background-color: rgb(68,74,89); }"
													 "QPushButton { color: rgb(100,164,32);}");
	connect(loadDatasetButton, &QPushButton::clicked, this, &EditorWidget::loadDatasetClickedSlot);
	connect (newDatasetButton, &QPushButton::clicked, this, &EditorWidget::newDatasetClickedSlot);
	datasetlayout->addItem(datasetTopSpacer,0,0,1,2);
	datasetlayout->addWidget(datasetLabel,1,0,1,2,Qt::AlignCenter);
	datasetlayout->addWidget(loadDatasetButton,2,0,Qt::AlignRight);
	datasetlayout->addWidget(newDatasetButton,2,1,Qt::AlignLeft);
	datasetlayout->addItem(datasetBottomSpacer,3,0,1,2);

	loadDatasetWindow = new LoadDatasetWindow(this);
	newDatasetWindow = new NewDatasetWindow(this);

	imageViewerContainer = new QWidget(this);
	imageViewerContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QGridLayout *containerlayout = new QGridLayout(imageViewerContainer);
	containerlayout->setMargin(0);
	imageViewer = new ImageViewer();
	imageViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	containerlayout->addWidget(imageViewer,0,0);
	//QImage img = QImage("../frame_25400.jpg");
	//imageViewer->setImage(img);

	keypointWidget = new KeypointWidget(horizontalSplitter);
	keypointWidget->hide();

	buttonWidget = new QWidget(mainSplitter);
	QGridLayout *buttonlayout = new QGridLayout(buttonWidget);
	buttonlayout->setSpacing(20);
	previousButton = new QPushButton("<< Previous");
	previousButton->setMinimumSize(130,35);
	previousButton->setMaximumSize(130,35);
	previousButton->setEnabled(false);
	connect(previousButton, &QPushButton::clicked, this, &EditorWidget::previousClickedSlot);
	nextButton = new QPushButton("Next >>");
	nextButton->setMinimumSize(130,35);
	nextButton->setMaximumSize(130,35);
	nextButton->setEnabled(false);
	connect(nextButton, &QPushButton::clicked, this, &EditorWidget::nextClickedSlot);
	QWidget *buttonSpacer1 = new QWidget(this);
	buttonSpacer1->setMaximumSize(100,100);
	buttonSpacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	zoomButton = new QPushButton("Zoom");
	zoomButton->setIcon(QIcon::fromTheme("search"));
	zoomButton->setCheckable(true);
	zoomButton->setMinimumSize(130,35);
	zoomButton->setMaximumSize(130,35);
	connect(zoomButton, &QPushButton::toggled, this, &EditorWidget::zoomToggled);
	connect(zoomButton, &QPushButton::toggled, this, &EditorWidget::zoomToggledSlot);
	panButton = new QPushButton("Pan");
	panButton->setIcon(QIcon::fromTheme("move"));
	panButton->setCheckable(true);
	panButton->setMinimumSize(130,35);
	panButton->setMaximumSize(130,35);
	connect(panButton, &QPushButton::toggled, this, &EditorWidget::panToggled);
	connect(panButton, &QPushButton::toggled, this, &EditorWidget::panToggledSlot);
	homeButton = new QPushButton("Home");
	homeButton->setIcon(QIcon::fromTheme("home"));
	homeButton->setMinimumSize(130,35);
	homeButton->setMaximumSize(130,35);
	connect(homeButton, &QPushButton::clicked, this, &EditorWidget::homeClicked);
	connect(homeButton, &QPushButton::clicked, this, &EditorWidget::homeClickedSlot);
	QWidget *buttonSpacer2 = new QWidget(this);
	buttonSpacer2->setMaximumSize(100,100);
	buttonSpacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	previousSetButton = new QPushButton("<< Previous Set");
	previousSetButton->setMinimumSize(160,35);
	previousSetButton->setMaximumSize(160,35);
	previousSetButton->setEnabled(false);
	connect(previousSetButton, &QPushButton::clicked, this, &EditorWidget::previousSetClickedSlot);
	nextSetButton = new QPushButton("Next Set >>");
	nextSetButton->setMinimumSize(160,35);
	nextSetButton->setMaximumSize(160,35);
	nextSetButton->setEnabled(false);
	connect(nextSetButton, &QPushButton::clicked, this, &EditorWidget::nextSetClickedSlot);
	QWidget *buttonSpacer3 = new QWidget(this);
	buttonSpacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	quitButton = new QPushButton("Quit");
	quitButton->setMinimumSize(130,35);
	quitButton->setMaximumSize(130,35);
	connect(quitButton, &QPushButton::clicked, this, &EditorWidget::quitClickedSlot);
	buttonlayout->addWidget(previousButton,0,0);
	buttonlayout->addWidget(nextButton,0,1);
	buttonlayout->addWidget(buttonSpacer1,0,2);
	buttonlayout->addWidget(zoomButton,0,3);
	buttonlayout->addWidget(panButton,0,4);
	buttonlayout->addWidget(homeButton,0,5);
	buttonlayout->addWidget(buttonSpacer2,0,6);
	buttonlayout->addWidget(previousSetButton,0,7);
	buttonlayout->addWidget(nextSetButton,0,8);
	buttonlayout->addWidget(buttonSpacer3,0,9);
	buttonlayout->addWidget(quitButton,0,10);

	stackedWidget->addWidget(datasetWidget);
	stackedWidget->addWidget(newDatasetWindow);
	stackedWidget->addWidget(mainSplitter);

	horizontalSplitter->addWidget(leftSplitter);
	horizontalSplitter->addWidget(imageViewerContainer);
	horizontalSplitter->addWidget(keypointWidget);
	horizontalSplitter->setSizes({200,1000,200});

	mainSplitter->addWidget(horizontalSplitter);
	mainSplitter->addWidget(buttonWidget);
	mainSplitter->setCollapsible(0,false);
	mainSplitter->setCollapsible(1,false);
	mainSplitter->setSizes({1000,50});

	layout->addWidget(stackedWidget,0,0),
	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals
	connect(imageViewer, &ImageViewer::zoomFinished, this, &EditorWidget::zoomFinishedSlot);
	connect(imageViewer, &ImageViewer::panFinished, this, &EditorWidget::panFinishedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, this, &EditorWidget::datasetLoadedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, this, &EditorWidget::datasetLoaded);
	connect(datasetControlWidget, &DatasetControlWidget::frameSelectionChanged, this, &EditorWidget::frameChangedSlot);
	connect(datasetControlWidget, &DatasetControlWidget::imgSetChanged, this, &EditorWidget::imgSetChangedSlot);


	//<- Outgoing Signals
	connect(this, &EditorWidget::zoomToggled, imageViewer, &ImageViewer::zoomToggledSlot);
	connect(this, &EditorWidget::panToggled, imageViewer, &ImageViewer::panToggledSlot);
	connect(this, &EditorWidget::homeClicked, imageViewer, &ImageViewer::homeClickedSlot);
	connect(this, &EditorWidget::imageTranformationChanged, imageViewer, &ImageViewer::imageTransformationChangedSlot);
	connect(this, &EditorWidget::keypointSizeChanged, imageViewer, &ImageViewer::keypointSizeChangedSlot);
	connect(this, &EditorWidget::keypointShapeChanged, imageViewer, &ImageViewer::keypointShapeChangedSlot);
	connect(this, &EditorWidget::colorMapChanged, imageViewer, &ImageViewer::colorMapChangedSlot);
	connect(this, &EditorWidget::frameChanged, keypointWidget, &KeypointWidget::frameChangedSlot);
	connect(this, &EditorWidget::frameChanged, reprojectionWidget, &ReprojectionWidget::calculateReprojectionSlot);
	connect(this, &EditorWidget::frameChanged, datasetControlWidget, &DatasetControlWidget::frameChangedSlot);

	//<-> Relayed Signals
	connect(keypointWidget, &KeypointWidget::currentEntityChanged, imageViewer, &ImageViewer::currentEntityChangedSlot);
	connect(keypointWidget, &KeypointWidget::currentBodypartChanged, imageViewer, &ImageViewer::currentBodypartChangedSlot);
	connect(keypointWidget, &KeypointWidget::toggleEntityVisible, imageViewer, &ImageViewer::toggleEntityVisibleSlot);
	connect(keypointWidget, &KeypointWidget::updateViewer, imageViewer, &ImageViewer::updateViewer);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, keypointWidget, &KeypointWidget::datasetLoadedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, reprojectionWidget, &ReprojectionWidget::datasetLoadedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, datasetControlWidget, &DatasetControlWidget::datasetLoadedSlot);
	connect(imageViewer, &ImageViewer::keypointAdded, keypointWidget, &KeypointWidget::keypointAddedSlot);
	connect(imageViewer, &ImageViewer::keypointRemoved, keypointWidget, &KeypointWidget::keypointRemovedSlot);
	connect(imageViewer, &ImageViewer::keypointCorrected, keypointWidget, &KeypointWidget::keypointCorrectedSlot);
	connect(imageViewer, &ImageViewer::alreadyAnnotated, keypointWidget, &KeypointWidget::alreadyAnnotatedSlot);
	connect(imageViewer, &ImageViewer::keypointChangedForReprojection, reprojectionWidget, &ReprojectionWidget::calculateReprojectionSlot);
	connect(reprojectionWidget, &ReprojectionWidget::reprojectedPoints, keypointWidget, &KeypointWidget::setKeypointsFromDatasetSlot);
	connect(reprojectionWidget, &ReprojectionWidget::reprojectionToolToggled, imageViewer, &ImageViewer::toggleReprojectionSlot);
}

void EditorWidget::splitterMovedSlot(int, int) {
	int maxWidth = stackedWidget->size().width();
	int maxHeight = stackedWidget->size().height();
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
		else if (m_currentFrameIndex == m_currentImgSet->numCameras - 2) {
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
		else if (m_currentFrameIndex == 1) {
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

void EditorWidget::zoomToggledSlot(bool toggle) {
	if (toggle) panButton->setChecked(false);
}

void EditorWidget::panToggledSlot(bool toggle) {
	if (toggle) zoomButton->setChecked(false);
}

void EditorWidget::homeClickedSlot() {
	zoomButton->setChecked(false);
	panButton->setChecked(false);
}

void EditorWidget::zoomFinishedSlot() {
	zoomButton->setChecked(false);
}

void EditorWidget::panFinishedSlot() {
	panButton->setChecked(false);
}

void EditorWidget::datasetLoadedSlot() {
	keypointWidget->init();
	leftSplitter->show();
	keypointWidget->show();
	m_currentImgSet = Dataset::dataset->imgSets()[0];
	m_currentImgSetIndex = 0;
	m_currentFrameIndex = 0;
	imageViewer->setFrame(m_currentImgSet, m_currentFrameIndex);
	stackedWidget->setCurrentWidget(mainSplitter);
	nextButton->setEnabled(true);
	nextSetButton->setEnabled(true);
	splitterMovedSlot(0,0);
	connect(Dataset::dataset, &Dataset::keypointStateChanged, datasetControlWidget, &DatasetControlWidget::keypointStateChangedSlot);
}

void EditorWidget::loadDatasetClickedSlot() {
	loadDatasetWindow->show();
}

void EditorWidget::newDatasetClickedSlot() {
	//newDatasetWindow->show();
	stackedWidget->setCurrentWidget(newDatasetWindow);
}

void EditorWidget::exitToMainPageSlot() {
	stackedWidget->setCurrentWidget(datasetWidget);
}

void EditorWidget::quitClickedSlot() {
	Dataset::dataset->save();
	emit quitClicked();
}
