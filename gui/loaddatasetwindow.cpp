/*****************************************************************************
 * File:		loaddatasetwindow.cpp
 * Created:		23. October 2020
 * Author:		Timo Hueser
 * Contact:		timo.hueser@gmail.com
 * Copyright:	2021 Timo Hueser
 * License:		GPL v3.0
 *****************************************************************************/

#include "loaddatasetwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QErrorMessage>


LoadDatasetWindow::LoadDatasetWindow(QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Load Dataset");
	this->resize(700,500);
	this->setMinimumSize(600,200);
	settings = new QSettings();
	QGridLayout *loaddatasetlayout = new QGridLayout(this);

	datasetFolderBox = new QGroupBox("Select Dataset Folder");
	QGridLayout *datasetfolderlayout = new QGridLayout(datasetFolderBox);
	datasetFolderEdit = new QLineEdit();
	settings->beginGroup("DatasetLoader");
	datasetFolderEdit->setText(settings->value("datasetFolder").toString());
	settings->endGroup();
	connect(datasetFolderEdit, &QLineEdit::textEdited, 
			this, &LoadDatasetWindow::updateCameraOrderList);
	datasetFolderButton = new QPushButton();
	datasetFolderButton->setMinimumSize(40,40);
	datasetFolderButton->setIcon(QIcon::fromTheme("folder"));
	connect(datasetFolderButton, &QPushButton::clicked,
			this, &LoadDatasetWindow::datasetFolderClickedSlot);
	datasetfolderlayout->addWidget(datasetFolderEdit,0,0);
	datasetfolderlayout->addWidget(datasetFolderButton,0,1);
	QWidget *buttonWidget = new QWidget(this);
	buttonWidget->setMaximumSize(10000,50);
	QGridLayout *buttonlayout = new QGridLayout(buttonWidget);
	QWidget *spacer = new QWidget(buttonWidget);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	loadDatasetButton = new QPushButton("Load Dataset");
	loadDatasetButton->setMinimumSize(40,40);
	loadDatasetButton->setIcon(QIcon::fromTheme("download"));
	connect(loadDatasetButton, &QPushButton::clicked, 
			this, &LoadDatasetWindow::loadDatasetClickedSlot);
	cancelButton = new QPushButton("Cancel");
	cancelButton->setMinimumSize(40,40);
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	connect(cancelButton, &QPushButton::clicked, 
			this, &LoadDatasetWindow::reject);
	buttonlayout->addWidget(spacer,0,0);
	buttonlayout->addWidget(cancelButton,0,1);
	buttonlayout->addWidget(loadDatasetButton,0,2);


	cameraOrderBox = new QGroupBox("Camera Order");
	QGridLayout *cameraorderlayout = new QGridLayout(cameraOrderBox);
	cameraOrderList = new QListWidget(cameraOrderBox);
	connect(cameraOrderList, &QListWidget::currentItemChanged, 
			this, &LoadDatasetWindow::currentItemChangedSlot);
	initCameraListFromSettings();
	upButton = new QPushButton();
	upButton->setMinimumSize(30,30);
	upButton->setIcon(QIcon::fromTheme("up"));
	connect(upButton, &QPushButton::clicked, 
			this, &LoadDatasetWindow::moveLabelUpSlot);
	downButton = new QPushButton();
	downButton->setMinimumSize(30,30);
	downButton->setIcon(QIcon::fromTheme("down"));
	connect(downButton, &QPushButton::clicked, 
			this, &LoadDatasetWindow::moveLabelDownSlot);
	cameraorderlayout->addWidget(cameraOrderList,0,0,1,2);
	cameraorderlayout->addWidget(upButton, 1,0);
	cameraorderlayout->addWidget(downButton,1,1);

	loaddatasetlayout->addWidget(datasetFolderBox,0,0);
	loaddatasetlayout->addWidget(cameraOrderBox,1,0);
	loaddatasetlayout->addWidget(buttonWidget,2,0);

}


void LoadDatasetWindow::datasetFolderClickedSlot() {
	QString currentPath;
	if (datasetFolderEdit->text() != "") {
		QDir curDir = QDir(datasetFolderEdit->text());
		curDir.cdUp();
		currentPath = curDir.path();
	}
	else {
		currentPath = QDir::homePath();
	}
	QString dir = QFileDialog::getExistingDirectory(
				      this, "Dataset Folder", currentPath,
					  QFileDialog::ShowDirsOnly | 
					  QFileDialog::DontResolveSymlinks);
	
	if (dir != "") {
		datasetFolderEdit->setText(dir);
		updateCameraOrderList(dir);
	}
}


void LoadDatasetWindow::loadDatasetClickedSlot() {
	if (datasetFolderEdit->text() != "") {
		Dataset::dataset = new Dataset(datasetFolderEdit->text(), 
									   m_cameraNames);
		if (Dataset::dataset->loadSuccessfull()) {
			settings->beginGroup("DatasetLoader");
			settings->setValue("datasetFolder", datasetFolderEdit->text());
			settings->endGroup();
			settings->beginGroup(datasetFolderEdit->text());
			settings->setValue("cameraNames", 
							   QVariant::fromValue(m_cameraNames));
			settings->endGroup();
			emit datasetLoaded();
			this->close();
		}
		else {
			QErrorMessage *msg = new QErrorMessage(this);
			msg->showMessage("Folder is not a valid Dataset Folder.");
		}
 }
}


void LoadDatasetWindow::initCameraListFromSettings() {
	m_cameraNames = QDir(datasetFolderEdit->text()).entryList(
													    QDir::AllDirs | 
														QDir::NoDotAndDotDot);
	settings->beginGroup(datasetFolderEdit->text());
	QList<QString> cameraNames = settings->value(
								     "cameraNames").value<QList<QString>>();
	settings->endGroup();
	if (cameraNames.size() == m_cameraNames.size()) {
		for (const auto cam : cameraNames) {
			if (!m_cameraNames.contains(cam)) {
				updateCameraOrderList(datasetFolderEdit->text());
				return;
			}
		}
		m_cameraNames = cameraNames;
		for (const auto& cam : m_cameraNames) {
			addItem(cam);
		}
	}
}


void LoadDatasetWindow::updateCameraOrderList(const QString& dir) {
	cameraOrderList->clear();
	m_cameraNames = QDir(dir).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for (const auto& cam : m_cameraNames) {
		if (QFile::exists(dir + "/" + cam + "/annotations.csv")) {
			addItem(cam);
		}

	}
}


void LoadDatasetWindow::moveLabelUpSlot() {
	int row = cameraOrderList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = cameraOrderList->takeItem(row);
	QListWidgetItem *seperatorItem = cameraOrderList->takeItem(row);
	int newRow = std::max(row-2,0);
	cameraOrderList->insertItem(newRow,item);
	cameraOrderList->insertItem(newRow+1,seperatorItem);
	cameraOrderList->setCurrentRow(newRow);
	m_cameraNames.move(row/2, newRow/2);
	cameraOrderList->setCurrentRow(newRow);
}


void LoadDatasetWindow::moveLabelDownSlot() {
	int row = cameraOrderList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = cameraOrderList->takeItem(row);
	QListWidgetItem *seperatorItem = cameraOrderList->takeItem(row);
	int newRow = std::min(row+2,cameraOrderList->count());
	cameraOrderList->insertItem(newRow,item);
	cameraOrderList->insertItem(newRow+1,seperatorItem);
	cameraOrderList->setCurrentRow(newRow);
	m_cameraNames.move(row/2, newRow/2);
	cameraOrderList->setCurrentRow(newRow);
}


void LoadDatasetWindow::addItem(const QString &text) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
	cameraOrderList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	cameraOrderList->addItem(seperatorItem);
}


void LoadDatasetWindow::currentItemChangedSlot(QListWidgetItem *current, 
											   QListWidgetItem *previous) {
	if (current != nullptr)  {
		current->setBackground(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackground(QColor(34, 36, 40));
	}
}
