/*****************************************************************
	* File:			  mainwindow.cpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "mainwindow.hpp"

#include <QGridLayout>
#include <QCloseEvent>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>


//This is a global funciton, probably not ideal to have it here
void createToolBarButton(QToolButton *button, QAction*action, QIcon icon, bool enabled,
			bool checkable, QSize minSize) {
	button->setMinimumSize(minSize);
	button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	action->setCheckable(checkable);
	action->setEnabled(enabled);
	action->setIcon(icon);
	button->setDefaultAction(action);
}


MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
	this->setMinimumSize(800,400);
	this->showMaximized();
	this->setWindowTitle("Annotation Tool");
	this->setWindowIcon(QIcon::fromTheme("hand"));
	statusBar()->showMessage(tr("Ready"), 2000);

	settingsWindow = new SettingsWindow(this);
	helpWindow = new HelpWindow(this);

	mainBar = new MainBar(this);
	this->addToolBar(mainBar);

	stackedWidget = new QStackedWidget(this);
	setCentralWidget(stackedWidget);

	datasetWidget = new QWidget(this);
	QGridLayout *datasetlayout = new QGridLayout(datasetWidget);
	datasetlayout->setSpacing(16);
	QSpacerItem *datasetTopSpacer = new QSpacerItem(100,200,QSizePolicy::Maximum,QSizePolicy::Maximum);
	QSpacerItem *datasetBottomSpacer = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);

	QLabel *datasetLabel = new QLabel("What do you want to do?");
	datasetLabel->setFont(QFont("Sans Serif", 22, QFont::Bold));
	datasetLabel->setStyleSheet("QLabel {color: rgb(32,100,164)}");

	loadDatasetButton = new QPushButton("Annotate Dataset");
	loadDatasetButton->setMinimumSize(450,80);
	loadDatasetButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	loadDatasetButton->setStyleSheet("QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
													 				 "QPushButton:hover { background-color: rgb(68,74,89); }"
													 				 "QPushButton { color: rgb(100,164,32);}");

	newDatasetButton = new QPushButton("Create new Dataset");
	newDatasetButton->setMinimumSize(450,80);
	newDatasetButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	newDatasetButton->setStyleSheet("QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
													 				"QPushButton:hover { background-color: rgb(68,74,89); }"
													 				"QPushButton { color: rgb(100,164,32);}");

	newCalibrationButton = new QPushButton("Create new Calibration");
	newCalibrationButton->setMinimumSize(450,80);
	newCalibrationButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	newCalibrationButton->setStyleSheet("QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
																			"QPushButton:hover { background-color: rgb(68,74,89); }"
																			"QPushButton { color: rgb(100,164,32);}");

	exportTrainigsetButton = new QPushButton("Export Trainingset");
	exportTrainigsetButton->setMinimumSize(450,80);
	exportTrainigsetButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	exportTrainigsetButton->setStyleSheet("QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
													 					 "QPushButton:hover { background-color: rgb(68,74,89); }"
													 				 	 "QPushButton { color: rgb(100,164,32);}");

	connect(loadDatasetButton, &QPushButton::clicked, this, &MainWindow::loadDatasetClickedSlot);
	connect(newDatasetButton, &QPushButton::clicked, this, &MainWindow::newDatasetClickedSlot);
	connect(newCalibrationButton, &QPushButton::clicked, this, &MainWindow::newCalibrationClickedSlot);
	connect(exportTrainigsetButton, &QPushButton::clicked, this, &MainWindow::exportTrainingsetClickedSlot);

	datasetlayout->addItem(datasetTopSpacer,0,0);
	datasetlayout->addWidget(datasetLabel,1,0,Qt::AlignCenter);
	datasetlayout->addWidget(loadDatasetButton,2,0,Qt::AlignCenter);
	datasetlayout->addWidget(newDatasetButton,3,0,Qt::AlignCenter);
	datasetlayout->addWidget(newCalibrationButton,4,0,Qt::AlignCenter);
	datasetlayout->addWidget(exportTrainigsetButton,5,0,Qt::AlignCenter);
	datasetlayout->addItem(datasetBottomSpacer,6,0);


	loadDatasetWindow = new LoadDatasetWindow(this);
	newDatasetWindow = new NewDatasetWindow(this);
	newCalibrationWidget = new NewCalibrationWidget(this);
	exportTrainingsetWidget = new ExportTrainingsetWidget(this);
	editorWidget = new EditorWidget(this);

	stackedWidget->addWidget(datasetWidget);
	stackedWidget->addWidget(newDatasetWindow);
	stackedWidget->addWidget(newCalibrationWidget);
	stackedWidget->addWidget(exportTrainingsetWidget);
	stackedWidget->addWidget(editorWidget);


	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals
	connect(mainBar, &MainBar::openSettingsWindow, this, &MainWindow::openSettingsWindowSlot);
	connect(mainBar, &MainBar::openHelpWindow, this, &MainWindow::openHelpWindowSlot);
	connect(editorWidget, &EditorWidget::quitClicked, this, &MainWindow::quitClickedSlot);
	connect(editorWidget, &EditorWidget::newSegmentLoaded, this, &MainWindow::datasetLoadedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, this, &MainWindow::datasetLoadedSlot);
	connect(mainBar, &MainBar::exitToMainPage, this, &MainWindow::exitToMainPageSlot);

	//<- Outgoing Signals

	//<-> Relayed Signals
	connect(settingsWindow, &SettingsWindow::imageTranformationChanged, editorWidget, &EditorWidget::imageTranformationChanged);
	connect(settingsWindow, &SettingsWindow::keypointSizeChanged, editorWidget, &EditorWidget::keypointSizeChanged);
	connect(settingsWindow, &SettingsWindow::keypointShapeChanged, editorWidget, &EditorWidget::keypointShapeChanged);
	connect(settingsWindow, &SettingsWindow::colorMapChanged, editorWidget, &EditorWidget::colorMapChanged);
	connect(settingsWindow, &SettingsWindow::minViewsChanged, editorWidget, &EditorWidget::minViewsChanged);
	connect(settingsWindow, &SettingsWindow::errorThresholdChanged, editorWidget, &EditorWidget::errorThresholdChanged);
	connect(editorWidget, &EditorWidget::datasetLoaded, settingsWindow, &SettingsWindow::datasetLoadedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, editorWidget, &EditorWidget::datasetLoadedSlot);
	connect(loadDatasetWindow, &LoadDatasetWindow::datasetLoaded, editorWidget, &EditorWidget::datasetLoaded);
}


void MainWindow::openSettingsWindowSlot() {
	settingsWindow->show();
	settingsWindow->raise();
}


void MainWindow::openHelpWindowSlot() {
	helpWindow->show();
	helpWindow->raise();
}


void MainWindow::loadDatasetClickedSlot() {
	loadDatasetWindow->exec();
}


void MainWindow::newDatasetClickedSlot() {
	stackedWidget->setCurrentWidget(newDatasetWindow);
}


void MainWindow::newCalibrationClickedSlot() {
	stackedWidget->setCurrentWidget(newCalibrationWidget);
}


void MainWindow::exportTrainingsetClickedSlot() {
	stackedWidget->setCurrentWidget(exportTrainingsetWidget);
}


void MainWindow::datasetLoadedSlot() {
	stackedWidget->setCurrentWidget(editorWidget);
}


void MainWindow::quitClickedSlot() {
	this->close();
}


void MainWindow::exitToMainPageSlot() {
	if (stackedWidget->currentWidget() == editorWidget) {
		Dataset::dataset->save();
	}
	stackedWidget->setCurrentWidget(datasetWidget);
}
