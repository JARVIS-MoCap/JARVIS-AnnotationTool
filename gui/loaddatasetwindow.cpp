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
	this->resize(600,400);
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
	datasetFolderButton = new QPushButton();
	datasetFolderButton->setMinimumSize(40,40);
	datasetFolderButton->setIcon(QIcon::fromTheme("folder"));
	connect(datasetFolderButton, &QPushButton::clicked, this, &LoadDatasetWindow::datasetFolderClickedSlot);
	loadDatasetButton = new QPushButton("Load Dataset");
	loadDatasetButton->setMinimumSize(40,40);
	loadDatasetButton->setIcon(QIcon::fromTheme("download"));
	connect(loadDatasetButton, &QPushButton::clicked, this, &LoadDatasetWindow::loadDatasetClickedSlot);
	datasetfolderlayout->addWidget(datasetFolderEdit,0,0);
	datasetfolderlayout->addWidget(datasetFolderButton,0,1);

	cameraOrderBox = new QGroupBox("Camera Order");
	QGridLayout *cameraorderlayout = new QGridLayout(cameraOrderBox);
	cameraOrderList = new QListWidget(cameraOrderBox);
	cameraOrderList->setAlternatingRowColors(true);
	initCameraListFromSettings();
	upButton = new QPushButton();
	upButton->setMinimumSize(35,35);
	upButton->setIcon(QIcon::fromTheme("up"));
	connect(upButton, &QPushButton::clicked, this, &LoadDatasetWindow::moveLabelUpSlot);
	downButton = new QPushButton();
	downButton->setMinimumSize(35,35);
	downButton->setIcon(QIcon::fromTheme("down"));
	connect(downButton, &QPushButton::clicked, this, &LoadDatasetWindow::moveLabelDownSlot);
	cameraorderlayout->addWidget(cameraOrderList,0,0,1,2);
	cameraorderlayout->addWidget(upButton, 1,0);
	cameraorderlayout->addWidget(downButton,1,1);

	loaddatasetlayout->addWidget(datasetFolderBox,0,0);
	loaddatasetlayout->addWidget(cameraOrderBox,1,0);
	loaddatasetlayout->addWidget(loadDatasetButton,2,0,Qt::AlignRight);
}

void LoadDatasetWindow::datasetFolderClickedSlot() {
	QString dir = QFileDialog::getExistingDirectory(this,"Dataset Folder", "./",
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "") {
		datasetFolderEdit->setText(dir);
	}
	updateCameraOrderList(dir);
}

void LoadDatasetWindow::loadDatasetClickedSlot() {
	if (datasetFolderEdit->text() != "") {
		settings->beginGroup("DatasetLoader");
		settings->setValue("datasetFolder", datasetFolderEdit->text());
		settings->endGroup();
		Dataset::dataset = new Dataset(datasetFolderEdit->text(), m_cameraNames);
		settings->beginGroup(datasetFolderEdit->text());
		settings->setValue("cameraNames", QVariant::fromValue(m_cameraNames));
		settings->endGroup();
		emit datasetLoaded();
		this->close();
 }
}

void LoadDatasetWindow::initCameraListFromSettings() {
	m_cameraNames = QDir(datasetFolderEdit->text()).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	settings->beginGroup(datasetFolderEdit->text());
	QList<QString> cameraNames = settings->value("cameraNames").value<QList<QString>>();
	settings->endGroup();
	std::cout << cameraNames.size() << std::endl;
	if (cameraNames.size() == m_cameraNames.size()) {
		for (const auto cam : cameraNames) {
			if (!m_cameraNames.contains(cam)) {
				updateCameraOrderList(datasetFolderEdit->text());
				return;
			}
		}
		m_cameraNames = cameraNames;
		for (const auto& cam : m_cameraNames) {
			cameraOrderList->addItem(cam);
		}
	}
}

void LoadDatasetWindow::updateCameraOrderList(const QString& dir) {
	cameraOrderList->clear();
	m_cameraNames = QDir(dir).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for (const auto& cam : m_cameraNames) {
		cameraOrderList->addItem(cam);
	}
}

void LoadDatasetWindow::moveLabelUpSlot() {
	int row = cameraOrderList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = cameraOrderList->takeItem(row);
	int newRow = std::max(row-1,0);
	cameraOrderList->insertItem(newRow,item);
	cameraOrderList->setCurrentRow(newRow);
	m_cameraNames.move(row, newRow);
}

void LoadDatasetWindow::moveLabelDownSlot() {
	int row = cameraOrderList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = cameraOrderList->takeItem(row);
	int newRow = std::min(row+1,cameraOrderList->count());
	cameraOrderList->insertItem(newRow,item);
	cameraOrderList->setCurrentRow(newRow);
	m_cameraNames.move(row, newRow);
}
