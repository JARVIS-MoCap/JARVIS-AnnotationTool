/*****************************************************************
 * File:			configurableitemlist.cpp
 * Created: 	08. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "configurableitemlist.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>


ConfigurableItemList::ConfigurableItemList(QString name, QWidget *parent) :
			m_name(name), QWidget(parent) {

	QGridLayout *labelselectorlayout = new QGridLayout(this);
	labelselectorlayout->setMargin(3);

	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	connect(itemSelectorList, &QListWidget::currentItemChanged, this, &ConfigurableItemList::currentItemChangedSlot);
	//itemSelectorList->setAlternatingRowColors(true);
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
		if (item->text() != "") {
			items.append(item->text());
		}
	}
	return items;
}


void ConfigurableItemList::itemSelectedSlot(QListWidgetItem *item) {
	QString label = QInputDialog::getText(this,m_name,"Change Label:", QLineEdit::Normal, item->text());
	if (label != "") {
		item->setText(label);
		emit itemsChanged(getItems());
	}
}

void ConfigurableItemList::moveItemUpSlot() {
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

void ConfigurableItemList::moveItemDownSlot() {
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


void ConfigurableItemList::addItemSlot() {
	QString label = QInputDialog::getText(this,m_name,"Enter Label:", QLineEdit::Normal);
	if (label != "") {
		addItem(label);
	}
	emit itemsChanged(getItems());
}

void ConfigurableItemList::removeItemSlot() {
	int row = itemSelectorList->currentRow();
	delete itemSelectorList->takeItem(row);
	delete itemSelectorList->takeItem(row);
	emit itemsChanged(getItems());
}

void ConfigurableItemList::addItem(const QString &text) {
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

void ConfigurableItemList::currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous) {
	if (current != nullptr) {
		current->setBackgroundColor(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackgroundColor(QColor(34, 36, 40));
	}
}
