/*------------------------------------------------------------
 *  loaddatasetwindow.cpp
 *  Created: 03. December 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "loaddatasetwindow.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QToolButton>
#include <QFileDialog>


LoadDatasetWindow::LoadDatasetWindow(QWidget *parent) : QWidget(parent, Qt::Window) {
	this->resize(800,600);
	this->setMinimumSize(600,200);
	settings = new QSettings();
	setWindowTitle("Load Dataset");
	QGridLayout *loaddatasetlayout = new QGridLayout(this);

	datasetFolderBox = new QGroupBox("Select Dataset Folder");
	QGridLayout *datasetfolderlayout = new QGridLayout(datasetFolderBox);
	datasetFolderEdit = new QLineEdit();
	settings->beginGroup("DatasetLoader");
	datasetFolderEdit->setText(settings->value("datasetFolder").toString());
	settings->endGroup();
	datasetFolderButton = new QPushButton("Path");
	connect(datasetFolderButton, &QPushButton::clicked, this, &LoadDatasetWindow::datasetFolderClickedSlot);
	loadDatasetButton = new QPushButton("Load Dataset");
	connect(loadDatasetButton, &QPushButton::clicked, this, &LoadDatasetWindow::loadDatasetClickedSlot);

	datasetfolderlayout->addWidget(datasetFolderEdit,0,0);
	datasetfolderlayout->addWidget(datasetFolderButton,0,1);
	datasetfolderlayout->addWidget(loadDatasetButton,1,0,1,2,Qt::AlignRight);

	loaddatasetlayout->addWidget(datasetFolderBox,0,0);
}

void LoadDatasetWindow::datasetFolderClickedSlot() {
	QString dir = QFileDialog::getExistingDirectory(this,"Dataset Folder", "./",
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "") {
		datasetFolderEdit->setText(dir);
	}
}

void LoadDatasetWindow::loadDatasetClickedSlot() {
	if (datasetFolderEdit->text() != "") {
		settings->beginGroup("DatasetLoader");
		settings->setValue("datasetFolder", datasetFolderEdit->text());
		settings->endGroup();
		Dataset::dataset = new Dataset(datasetFolderEdit->text());
		emit datasetLoaded();
		this->close();
 }
}
