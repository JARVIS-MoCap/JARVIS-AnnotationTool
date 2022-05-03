/*****************************************************************
	* File:			  cameranameslist.cpp
	* Created: 	  16. February 2022
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "cameranameslist.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>


CameraNamesList::CameraNamesList(QString name, QWidget *parent) :
			QWidget(parent), m_name(name) {

	QGridLayout *labelselectorlayout = new QGridLayout(this);
	labelselectorlayout->setMargin(3);

	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	connect(itemSelectorList, &QListWidget::currentItemChanged, this, &CameraNamesList::currentItemChangedSlot);
	connect(itemSelectorList, &QListWidget::itemDoubleClicked, this, &CameraNamesList::itemSelectedSlot);
	moveItemUpButton = new QPushButton();
	moveItemUpButton->setIcon(QIcon::fromTheme("up"));
	moveItemUpButton->setMinimumSize(35,35);
	connect(moveItemUpButton, &QPushButton::clicked, this, &CameraNamesList::moveItemUpSlot);
	moveItemDownButton = new QPushButton();
	moveItemDownButton->setIcon(QIcon::fromTheme("down"));
	moveItemDownButton->setMinimumSize(35,35);
	connect(moveItemDownButton, &QPushButton::clicked, this, &CameraNamesList::moveItemDownSlot);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &CameraNamesList::addItemSlot);
	deleteItemButton = new QPushButton();
	deleteItemButton->setIcon(QIcon::fromTheme("discard"));
	deleteItemButton->setMinimumSize(35,35);
	connect(deleteItemButton, &QPushButton::clicked, this, &CameraNamesList::removeItemSlot);
	labelselectorlayout->addWidget(itemSelectorList,0,0,1,4);
	labelselectorlayout->addWidget(moveItemUpButton,1,0);
	labelselectorlayout->addWidget(moveItemDownButton,1,1);
	labelselectorlayout->addWidget(addItemButton,1,2);
	labelselectorlayout->addWidget(deleteItemButton,1,3);
	this->setLayout(labelselectorlayout);
}

QList<QString> CameraNamesList::getItems() {
	QList <QString> items;
	for (const auto& item : itemSelectorList->findItems("",Qt::MatchContains)) {
		if (item->text() != "") {
			items.append(item->text());
		}
	}
	return items;
}


void CameraNamesList::itemSelectedSlot(QListWidgetItem *item) {
	QString label = QInputDialog::getText(this,m_name,"Change Label:", QLineEdit::Normal, item->text());
	if (label != "") {
		if(!getItems().contains(label)) {
			item->setText(label);
			emit itemsChanged(getItems());
		}
		else {
			QErrorMessage *m_errorMsg = new QErrorMessage(this);
			m_errorMsg->showMessage("Name already taken!");
		}
	}
}


void CameraNamesList::moveItemUpSlot() {
	int row = itemSelectorList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	QListWidgetItem *seperatorItem = itemSelectorList->takeItem(row);
	int newRow = std::max(row-2,0);
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->insertItem(newRow+1,seperatorItem);
	itemSelectorList->setCurrentRow(newRow);
	emit itemsChanged(getItems());
}


void CameraNamesList::moveItemDownSlot() {
	int row = itemSelectorList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	QListWidgetItem *seperatorItem = itemSelectorList->takeItem(row);
	int newRow = std::min(row+2,itemSelectorList->count());
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->insertItem(newRow+1,seperatorItem);
	itemSelectorList->setCurrentRow(newRow);
	emit itemsChanged(getItems());
}


void CameraNamesList::addItemSlot() {
	if (m_cameras.size() == 0) {
		QErrorMessage *errorMsg = new QErrorMessage(this);
		errorMsg->showMessage("Please click update camera names first!");
		return;
	}
	QList<QString> notAddedCameras = m_cameras;
	for (const auto & cam : getItems()) {
		if (notAddedCameras.contains(cam)) {
			notAddedCameras.removeAll(cam);
		}
	}
	if (notAddedCameras.size() == 0) {
		QErrorMessage *errorMsg = new QErrorMessage(this);
		errorMsg->showMessage("All cameras already in list!");
		return;
	}
	bool accepted;
	QString label = QInputDialog::getItem(this,"Add single Camera","Select Camera", notAddedCameras,0, false, &accepted);
	if (accepted) {
		if (label != "") {
			if(!getItems().contains(label)) {
				addItem(label);
			}
			else {
				QErrorMessage *errorMsg = new QErrorMessage(this);
				errorMsg->showMessage("Camera was added already!");
			}
		}
		emit itemsChanged(getItems());
	}
}


void CameraNamesList::removeItemSlot() {
	int row = itemSelectorList->currentRow();
	delete itemSelectorList->takeItem(row);
	delete itemSelectorList->takeItem(row);
	emit itemsChanged(getItems());
}


void CameraNamesList::addItem(const QString &text) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
	itemSelectorList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	itemSelectorList->addItem(seperatorItem);
	emit itemsChanged(getItems());
}


void CameraNamesList::currentItemChangedSlot(QListWidgetItem *current,
			QListWidgetItem *previous) {
	if (current != nullptr) {
		current->setBackground(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackground(QColor(34, 36, 40));
	}
}

void CameraNamesList::addCameras(QList<QString> cameras) {
	m_cameras = cameras;
}
