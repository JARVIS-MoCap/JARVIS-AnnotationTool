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


ConfigurableItemList::ConfigurableItemList(QString name, DatasetConfig *datasetConfig, QWidget *parent) :
			m_name(name), m_datasetConfig(datasetConfig), QWidget(parent) {
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


void ConfigurableItemList::addItemSlot() {
	QStringList items;
	items.append(QInputDialog::getText(this,m_name,"Enter Label:", QLineEdit::Normal));
	if (!items.isEmpty()) {
		for (const auto & item : items) {
			itemSelectorList->addItem(item);
		}
	}
}

void ConfigurableItemList::removeItemSlot() {
	delete itemSelectorList->takeItem(itemSelectorList->currentRow());
}
