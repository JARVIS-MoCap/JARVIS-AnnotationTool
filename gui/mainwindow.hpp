/*------------------------------------------------------------
 *  mainwindow.hpp
 *  Created: 23. October 2020
 *  Author:  Timo Hüser
 *------------------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.hpp"
#include "mainbar.hpp"
#include "settingswindow.hpp"
#include "helpwindow.hpp"
#include "loaddatasetwindow.hpp"
#include "newdatasetwindow.hpp"
#include "newcalibrationwidget.hpp"
#include "exportdatasetwidget.hpp"
#include "editorwidget.hpp"

#include <QMainWindow>
#include <QStackedWidget>


class MainWindow : public QMainWindow {
	Q_OBJECT
	public:
		explicit MainWindow(QMainWindow *parent = nullptr);

	public slots:
		void openSettingsWindowSlot();
		void openHelpWindowSlot();
		void quitClickedSlot();

	private:
		SettingsWindow *settingsWindow;
		HelpWindow *helpWindow;

		MainBar *mainBar;
		QStackedWidget *stackedWidget;

		QWidget *datasetWidget;
		QPushButton *loadDatasetButton;
		QPushButton *newDatasetButton;
		QPushButton *newCalibrationButton;
		QPushButton *exportDatasetButton;

		LoadDatasetWindow *loadDatasetWindow;
		NewDatasetWindow *newDatasetWindow;
		NewCalibrationWidget *newCalibrationWidget;
		ExportDatasetWidget *exportDatasetWidget;
		EditorWidget *editorWidget;

	private slots:
		void loadDatasetClickedSlot();
		void newDatasetClickedSlot();
		void newCalibrationClickedSlot();
		void exportDatasetClickedSlot();
		void datasetLoadedSlot();
		void exitToMainPageSlot();
};

#endif
