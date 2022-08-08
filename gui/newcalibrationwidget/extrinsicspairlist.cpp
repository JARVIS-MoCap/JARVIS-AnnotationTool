/*****************************************************************
	* File:			  extrinsicspairlist.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "extrinsicspairlist.hpp"
#include "paircreatorwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>


ExtrinsicsPairList::ExtrinsicsPairList(QString name, QWidget *parent) :
			QWidget(parent), m_name(name) {
	QGridLayout *labelselectorlayout = new QGridLayout(this);
	labelselectorlayout->setContentsMargins(3,3,3,3);
	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	connect(itemSelectorList, &QListWidget::currentItemChanged, this, &ExtrinsicsPairList::currentItemChangedSlot);
	connect(itemSelectorList, &QListWidget::itemDoubleClicked, this, &ExtrinsicsPairList::itemDoubleClickedSlot);
	moveItemUpButton = new QPushButton();
	moveItemUpButton->setIcon(QIcon::fromTheme("up"));
	moveItemUpButton->setMinimumSize(35,35);
	connect(moveItemUpButton, &QPushButton::clicked, this, &ExtrinsicsPairList::moveItemUpSlot);
	moveItemDownButton = new QPushButton();
	moveItemDownButton->setIcon(QIcon::fromTheme("down"));
	moveItemDownButton->setMinimumSize(35,35);
	connect(moveItemDownButton, &QPushButton::clicked, this, &ExtrinsicsPairList::moveItemDownSlot);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &ExtrinsicsPairList::addItemSlot);
	deleteItemButton = new QPushButton();
	deleteItemButton->setIcon(QIcon::fromTheme("discard"));
	deleteItemButton->setMinimumSize(35,35);
	connect(deleteItemButton, &QPushButton::clicked, this, &ExtrinsicsPairList::removeItemSlot);
	labelselectorlayout->addWidget(itemSelectorList,0,0,1,4);
	labelselectorlayout->addWidget(moveItemUpButton,1,0);
	labelselectorlayout->addWidget(moveItemDownButton,1,1);
	labelselectorlayout->addWidget(addItemButton,1,2);
	labelselectorlayout->addWidget(deleteItemButton,1,3);
	this->setLayout(labelselectorlayout);
}


QList<QList<QString>> ExtrinsicsPairList::getItems() {
	return m_cameraPairs;
}


void ExtrinsicsPairList::setItems(QList<QList<QString>> items) {
	m_cameraPairs.clear();
	itemSelectorList->clear();
	for (const auto & newPair : items) {
		if (newPair.size() == 2) {
			m_cameraPairs.append(newPair);
			addItem(newPair[0] + " --> " + newPair[1]);
		}
		else if (newPair.size() == 3) {
			m_cameraPairs.append(newPair);
			addItem(newPair[0] + " --> " + newPair[1] + " --> " + newPair[2]);
		}
	}
	itemSelectorList->setCurrentRow(0);
}


void ExtrinsicsPairList::moveItemUpSlot() {
	int row = itemSelectorList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	QListWidgetItem *seperatorItem = itemSelectorList->takeItem(row);
	int newRow = std::max(row-2,0);
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->insertItem(newRow+1,seperatorItem);
	m_cameraPairs.move(itemSelectorList->currentRow()/2, newRow/2);
	itemSelectorList->setCurrentRow(newRow);
}


void ExtrinsicsPairList::moveItemDownSlot() {
	int row = itemSelectorList->currentRow();
	if (row == -1) return;
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	QListWidgetItem *seperatorItem = itemSelectorList->takeItem(row);
	int newRow = std::min(row+2,itemSelectorList->count());
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->insertItem(newRow+1,seperatorItem);
	m_cameraPairs.move(itemSelectorList->currentRow()/2, newRow/2);
	itemSelectorList->setCurrentRow(newRow);
}


void ExtrinsicsPairList::addItemSlot() {
	PairCreatorWindow* pairCreatorWindow = new PairCreatorWindow(m_cameraNames, this);
	pairCreatorWindow->exec();
	QList<QString> newPair = pairCreatorWindow->getCameraPair();
	if (m_cameraPairs.contains(newPair)) {
		QErrorMessage *m_errorMsg = new QErrorMessage(this);
		m_errorMsg->showMessage("Camera Pair exists already!");
		return;
	}
	if (newPair.size() == 2) {
		m_cameraPairs.append(newPair);
		addItem(newPair[0] + " --> " + newPair[1]);
	}
	else if (newPair.size() == 3) {
		m_cameraPairs.append(newPair);
		addItem(newPair[0] + " --> " + newPair[1] + " --> " + newPair[2]);
	}
}


void ExtrinsicsPairList::removeItemSlot() {
	int row = itemSelectorList->currentRow();
	delete itemSelectorList->takeItem(row);
	delete itemSelectorList->takeItem(row);
	m_cameraPairs.removeAt(row/2);
}

void ExtrinsicsPairList::cameraNamesChangedSlot(QList<QString> cameraNames) {
	m_cameraNames = cameraNames;
	QList<QList<QString>> validPairs = m_cameraPairs;
	for (const auto &pair : m_cameraPairs) {
		for (const auto &cam : pair) {
			if (!cameraNames.contains(cam)) {
				validPairs.removeAll(pair);
				break;
			}
		}
	}
	m_cameraPairs = validPairs;
	setItems(m_cameraPairs);
}


void ExtrinsicsPairList::addItem(const QString &text) {
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
}


void ExtrinsicsPairList::currentItemChangedSlot(QListWidgetItem *current,
			QListWidgetItem *previous) {
	if (current != nullptr)  {
		current->setBackground(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackground(QColor(34, 36, 40));
	}
}


void ExtrinsicsPairList::itemDoubleClickedSlot(QListWidgetItem *item) {
	QList<QString> pair = m_cameraPairs[itemSelectorList->row(item)/2];
	PairCreatorWindow* pairCreatorWindow = new PairCreatorWindow(m_cameraNames, this);
	pairCreatorWindow->setCameraPair(pair);
	pairCreatorWindow->exec();
	QList<QString> newPair = pairCreatorWindow->getCameraPair();
	if (m_cameraPairs.contains(newPair)) {
		QErrorMessage *m_errorMsg = new QErrorMessage(this);
		m_errorMsg->showMessage("Camera Pair exists already!");
		return;
	}
	if (newPair.size() == 2) {
		m_cameraPairs[itemSelectorList->row(item)/2] = newPair;
		item->setText(newPair[0] + " --> " + newPair[1]);
	}
	else if (newPair.size() == 3) {
		m_cameraPairs[itemSelectorList->row(item)/2] = newPair;
		item->setText(newPair[0] + " --> " + newPair[1] + " --> " + newPair[2]);
	}
}
