/*******************************************************************************
 * File:			  mainwindow.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.hpp"
#include "mainbar.hpp"
#include "settingswindow.hpp"
#include "loaddatasetwindow.hpp"
#include "newdatasetwindow.hpp"
#include "newcalibrationwidget.hpp"
#include "exporttrainingsetwidget.hpp"
#include "editorwidget.hpp"

#include <QMainWindow>
#include <QStackedWidget>


class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit MainWindow(QMainWindow *parent = nullptr);

	public slots:
		void openSettingsWindowSlot();
		void quitClickedSlot();

	private:
		SettingsWindow *settingsWindow;

		MainBar *mainBar;
		QStackedWidget *stackedWidget;

		QWidget *datasetWidget;
		QPushButton *loadDatasetButton;
		QPushButton *newDatasetButton;
		QPushButton *newCalibrationButton;
		QPushButton *exportTrainigsetButton;

		LoadDatasetWindow *loadDatasetWindow;
		NewDatasetWindow *newDatasetWindow;
		NewCalibrationWidget *newCalibrationWidget;
		ExportTrainingsetWidget *exportTrainingsetWidget;
		EditorWidget *editorWidget;

	private slots:
		void loadDatasetClickedSlot();
		void newDatasetClickedSlot();
		void newCalibrationClickedSlot();
		void exportTrainingsetClickedSlot();
		void datasetLoadedSlot(bool isSetupAnnotation, QString selectedSegment);
		void exitToMainPageSlot();
};

#endif
