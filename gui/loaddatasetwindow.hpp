/*****************************************************************
	* File:			  loaddatasetwindow.hpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef LOADDATASETWINDOW_H
#define LOADDATASETWINDOW_H

#include "globals.hpp"
#include "dataset.hpp"

#include <QToolButton>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QToolBar>
#include <QGroupBox>
#include <QListWidget>
#include <QDialog>


class LoadDatasetWindow : public QDialog {
	Q_OBJECT

	public:
		explicit LoadDatasetWindow(QWidget *parent = nullptr);

	signals:
		void datasetLoaded();

	private:
		void initCameraListFromSettings();

		QSettings *settings;

		QGroupBox *datasetFolderBox;
		QLineEdit *datasetFolderEdit;
		QPushButton *datasetFolderButton;
		QPushButton *loadDatasetButton;
		QPushButton *cancelButton;

		QGroupBox *cameraOrderBox;
		QListWidget *cameraOrderList;
		QPushButton *upButton;
		QPushButton *downButton;

		QList<QString> m_cameraNames;

		void addItem(const QString &text);

	private slots:
		void datasetFolderClickedSlot();
		void loadDatasetClickedSlot();
		void moveLabelUpSlot();
		void moveLabelDownSlot();
		void updateCameraOrderList(const QString& dir);
		void currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif
