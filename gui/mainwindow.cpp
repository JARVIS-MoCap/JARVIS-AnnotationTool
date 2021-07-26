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

	editorWidget = new EditorWidget(this);
	setCentralWidget(editorWidget);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals
	connect(mainBar, &MainBar::openSettingsWindow, this, &MainWindow::openSettingsWindowSlot);
	connect(editorWidget, &EditorWidget::quitClicked, this, &MainWindow::quitClickedSlot);
	connect(settingsWindow, &SettingsWindow::imageTranformationChanged, editorWidget, &EditorWidget::imageTranformationChanged);
	connect(settingsWindow, &SettingsWindow::keypointSizeChanged, editorWidget, &EditorWidget::keypointSizeChanged);
	connect(settingsWindow, &SettingsWindow::keypointShapeChanged, editorWidget, &EditorWidget::keypointShapeChanged);
	connect(settingsWindow, &SettingsWindow::colorMapChanged, editorWidget, &EditorWidget::colorMapChanged);
	connect(editorWidget, &EditorWidget::datasetLoaded, settingsWindow, &SettingsWindow::datasetLoadedSlot);
	//<- Outgoing Signals

	//<-> Relayed Signals
	connect(mainBar, &MainBar::exitToMainPage, editorWidget, &EditorWidget::exitToMainPageSlot);
	connect(editorWidget, &EditorWidget::toggleExitButton, mainBar, &MainBar::toggleExitButtonSlot);


}

void MainWindow::openSettingsWindowSlot() {
	settingsWindow->show();
}

void MainWindow::quitClickedSlot() {
	this->close();
}

void MainWindow::closeEvent (QCloseEvent *event) {
}
