/*------------------------------------------------------------
 *  mainwindow.cpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "mainwindow.hpp"

#include <QGridLayout>
#include <QCloseEvent>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>

/*const QMap<QString, QFont> fonts = {{"default", QFont("Sans Serif", 11)},
																		{"bold", QFont("Sans Serif", 11, QFont::Bold)},
																		{"big", QFont("Sans Serif", 12, QFont::Bold)},
																		{"verybig", QFont("Sans Serif", 14, QFont::Bold)},
																		{"headline", QFont("Sans Serif", 20, QFont::Bold)},
																		{"infotext", QFont("Sans Serif", 24, QFont::Bold)},
																		{"unit", QFont("Sans Serif", 10, QFont::Bold)}};*/


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
	this->setMinimumSize(800,800);
	this->showMaximized();
	this->setWindowTitle("Annotation Tool");
	this->setWindowIcon(QIcon("icons/hand.png"));
	statusBar()->showMessage(tr("Ready"), 2000);

	settingsWindow = new SettingsWindow(this);

	mainBar = new MainBar(this);
	this->addToolBar(mainBar);

	stackedWidget = new QStackedWidget(this);
	setCentralWidget(stackedWidget);

	datasetWidget = new QWidget(this);
	QSpacerItem *datasetTopSpacer = new QSpacerItem(100,300,QSizePolicy::Maximum,QSizePolicy::Maximum);
	QSpacerItem *datasetBottomSpacer = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
	QGridLayout *datasetlayout = new QGridLayout(datasetWidget);
	datasetlayout->setSpacing(16);
	QLabel *datasetLabel = new QLabel("What do you want to do?");
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
	 newCalibrationButton = new QPushButton("New Calibration");
	 newCalibrationButton->setMinimumSize(450,80);
	 newCalibrationButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
	 newCalibrationButton->setStyleSheet(" QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
														"QPushButton:hover { background-color: rgb(68,74,89); }"
														"QPushButton { color: rgb(100,164,32);}");
		exportDatasetButton = new QPushButton("Export Dataset");
		exportDatasetButton->setMinimumSize(450,80);
		exportDatasetButton->setFont(QFont("Sans Serif", 18, QFont::Bold));
		exportDatasetButton->setStyleSheet(" QPushButton { border-radius: 10px; border: 4px solid rgb(100,164,32); }"
														 "QPushButton:hover { background-color: rgb(68,74,89); }"
														 "QPushButton { color: rgb(100,164,32);}");
	connect(loadDatasetButton, &QPushButton::clicked, this, &MainWindow::loadDatasetClickedSlot);
	connect (newDatasetButton, &QPushButton::clicked, this, &MainWindow::newDatasetClickedSlot);
	connect (newCalibrationButton, &QPushButton::clicked, this, &MainWindow::newCalibrationClickedSlot);
	connect (exportDatasetButton, &QPushButton::clicked, this, &MainWindow::exportDatasetClickedSlot);
	datasetlayout->addItem(datasetTopSpacer,0,0,1,2);
	datasetlayout->addWidget(datasetLabel,1,0,1,2,Qt::AlignCenter);
	datasetlayout->addWidget(loadDatasetButton,2,0,Qt::AlignRight);
	datasetlayout->addWidget(newDatasetButton,2,1,Qt::AlignLeft);
	datasetlayout->addWidget(newCalibrationButton,3,0,Qt::AlignRight);
	datasetlayout->addWidget(exportDatasetButton,3,1,Qt::AlignLeft);
	datasetlayout->addItem(datasetBottomSpacer,4,0,1,2);


	loadDatasetWindow = new LoadDatasetWindow(this);
	newDatasetWindow = new NewDatasetWindow(this);
	newCalibrationWidget = new NewCalibrationWidget(this);
	exportDatasetWidget = new ExportDatasetWidget(this);
	editorWidget = new EditorWidget(this);

	stackedWidget->addWidget(datasetWidget);
	stackedWidget->addWidget(newDatasetWindow);
	stackedWidget->addWidget(newCalibrationWidget);
	stackedWidget->addWidget(exportDatasetWidget);
	stackedWidget->addWidget(editorWidget);


	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals
	connect(mainBar, &MainBar::openSettingsWindow, this, &MainWindow::openSettingsWindowSlot);
	connect(editorWidget, &EditorWidget::quitClicked, this, &MainWindow::quitClickedSlot);
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
}

void MainWindow::loadDatasetClickedSlot() {
	loadDatasetWindow->show();
}

void MainWindow::newDatasetClickedSlot() {
	stackedWidget->setCurrentWidget(newDatasetWindow);
}

void MainWindow::newCalibrationClickedSlot() {
	stackedWidget->setCurrentWidget(newCalibrationWidget);
}

void MainWindow::exportDatasetClickedSlot() {
	stackedWidget->setCurrentWidget(exportDatasetWidget);
}

void MainWindow::datasetLoadedSlot() {
	stackedWidget->setCurrentWidget(editorWidget);
}

void MainWindow::quitClickedSlot() {
	this->close();
}

void MainWindow::exitToMainPageSlot() {
	if (stackedWidget->currentWidget() == editorWidget) {
		std::cout << "Saving Dataset" << std::endl;
		Dataset::dataset->save();
	}
	stackedWidget->setCurrentWidget(datasetWidget);
}

void MainWindow::closeEvent (QCloseEvent *event) {
}
