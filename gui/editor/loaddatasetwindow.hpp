/******************************************************************************
 * File:		loaddatasetwindow.hpp
 * Created:		23. October 2020
 * Author:		Timo Hueser
 * Contact: 	timo.hueser@gmail.com
 * Copyright:	2021 Timo Hueser
 * License:		GPL v3.0
 *****************************************************************************/

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
#include <QGroupBox>
#include <QTreeWidget>
#include <QDialog>
#include <QListWidget>

#include "yaml-cpp/yaml.h"


class LoadDatasetWindow : public QDialog {
	Q_OBJECT

	public:
		explicit LoadDatasetWindow(QWidget *parent = nullptr);

	signals:
		void datasetLoaded();

	private:
		QSettings *settings;

		QLineEdit *datasetFileEdit;
		QPushButton *datasetFileButton;
		QLineEdit *selectedSegmentEdit;
		QPushButton *loadDatasetButton;
		QPushButton *cancelButton;

		QTreeWidget *datasetSegmentsTree;

		QListWidget *cameraOrderList;
		QPushButton *upButton;
		QPushButton *downButton;

		QList<QString> m_cameraNames;
		QList<SkeletonComponent> m_skeleton;

		YAML::Node m_datasetYaml;
		QString m_datasetFolder;
		QList<QString> m_segments;

		void addItem(const QString &text);
		void updateCameraOrderList();
		void updateDatasetSegmentTree();


	private slots:
		void datasetFileClickedSlot();
		void datasetFileEditedSlot(const QString &path);
		void loadDatasetClickedSlot();
		void moveLabelUpSlot();
		void moveLabelDownSlot();
		void currentItemChangedSlot(QListWidgetItem *current,
									QListWidgetItem *previous);
		void datasetSegmentChangedSlot(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

#endif
