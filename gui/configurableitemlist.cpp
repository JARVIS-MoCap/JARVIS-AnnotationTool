/*****************************************************************
 * File:			configurableitemlist.cpp
 * Created: 	08. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "newdatasetwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>


ConfigurableItemList::ConfigurableItemList(QString name, DatasetConfig *datasetConfig, bool pathMode, QWidget *parent) :
			m_pathMode(pathMode), m_name(name), m_datasetConfig(datasetConfig), QWidget(parent) {
	QGridLayout *labelselectorlayout = new QGridLayout(this);
	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	itemSelectorList->setAlternatingRowColors(true);
	connect(itemSelectorList, &QListWidget::itemDoubleClicked, this, &ConfigurableItemList::itemSelectedSlot);
	moveItemUpButton = new QPushButton();
	moveItemUpButton->setIcon(QIcon::fromTheme("up"));
	moveItemUpButton->setMinimumSize(35,35);
	connect(moveItemUpButton, &QPushButton::clicked, this, &ConfigurableItemList::moveItemUpSlot);
	moveItemDownButton = new QPushButton();
	moveItemDownButton->setIcon(QIcon::fromTheme("down"));
	moveItemDownButton->setMinimumSize(35,35);
	connect(moveItemDownButton, &QPushButton::clicked, this, &ConfigurableItemList::moveItemDownSlot);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &ConfigurableItemList::addItemSlot);
	deleteItemButton = new QPushButton();
	deleteItemButton->setIcon(QIcon::fromTheme("discard"));
	deleteItemButton->setMinimumSize(35,35);
	connect(deleteItemButton, &QPushButton::clicked, this, &ConfigurableItemList::removeItemSlot);
	labelselectorlayout->addWidget(itemSelectorList,0,0,1,4);
	labelselectorlayout->addWidget(moveItemUpButton,1,0);
	labelselectorlayout->addWidget(moveItemDownButton,1,1);
	labelselectorlayout->addWidget(addItemButton,1,2);
	labelselectorlayout->addWidget(deleteItemButton,1,3);
	this->setLayout(labelselectorlayout);
}

QList<QString> ConfigurableItemList::getItems() {
	QList <QString> items;
	for (const auto& item : itemSelectorList->findItems("",Qt::MatchContains)) {
		items.append(item->text());
	}
	return items;
}


void ConfigurableItemList::itemSelectedSlot(QListWidgetItem *item) {
}

void ConfigurableItemList::moveItemUpSlot() {
	int row = itemSelectorList->currentRow();
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	int newRow = std::max(row-1,0);
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->setCurrentRow(newRow);
}

void ConfigurableItemList::moveItemDownSlot() {
	int row = itemSelectorList->currentRow();
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	int newRow = std::min(row+1,itemSelectorList->count());
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->setCurrentRow(newRow);
}

int ConfigurableItemList::getNumberSubfolders(QString path) {
	int count = 0;
	for (QDirIterator it(path); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (m_datasetConfig->dataType == "Images" && QDir(subpath).exists() && suffix != "." && suffix != "..") count++;
		if (m_datasetConfig->dataType == "Videos" &&  subpath.split('.').takeLast() == "avi") count++;
	}
	return count;
}

bool ConfigurableItemList::isValidRecordingFolder(QString path) {
	bool isValid = true;
	if (getNumberSubfolders(path) == m_datasetConfig->numCameras) {
		if (m_datasetConfig->dataType == "Images") {
			for (QDirIterator it(path); it.hasNext();) {
				QString path = it.next();
				QString suffix = path.split('/').takeLast();
				if (suffix != "." && suffix != "..") {
					if (getNumberSubfolders(path) != 0) isValid = false;
				}
			}
		}
	}
	else {
		isValid = false;
	}
	return isValid;
}

void ConfigurableItemList::addItemSlot() {
	QStringList items;
	if (m_pathMode) {
		QString dir = QFileDialog::getExistingDirectory(this,m_name, "/home/trackingsetup/Documents/Sabine_17062021",
					QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if (isValidRecordingFolder(dir)) {
			items.append(dir);
		}
		for (QDirIterator it(dir,  QDirIterator::Subdirectories); it.hasNext();) {
			QString path = it.next();
			QString suffix = path.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				if (isValidRecordingFolder(path)) {
					items.append(path);
				}
			}
		}
	}
	else {
		items.append(QInputDialog::getText(this,m_name,"Enter Label:", QLineEdit::Normal));
	}
	if (!items.isEmpty()) {
		for (const auto & item : items) {
			itemSelectorList->addItem(item);
		}
	}
}

void ConfigurableItemList::removeItemSlot() {
	delete itemSelectorList->takeItem(itemSelectorList->currentRow());
}
