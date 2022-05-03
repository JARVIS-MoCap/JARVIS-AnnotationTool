/*****************************************************************
	* File:			  checkableitemlist.cpp
	* Created: 	  12. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "checkableitemlist.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>


CheckableItemList::CheckableItemList(QString name, QWidget *parent) :
			QWidget(parent), m_name(name) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(3);

	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	connect(itemSelectorList, &QListWidget::itemChanged, this, &CheckableItemList::checkStateChangedSlot);
	layout->addWidget(itemSelectorList,0,0);
}

QList<QPair<QString, bool>> CheckableItemList::getItems() {
	QList<QPair<QString, bool>> items;
	for (const auto& item : itemSelectorList->findItems("",Qt::MatchContains)) {
		if (item->text() != "") {
			QPair<QString, bool> pair;
			pair.first = item->text();
			if (item->checkState() == Qt::Checked) {
				pair.second = true;
			}
			else {
				pair.second = false;
			}
			items.append(pair);
		}
	}
	return items;
}


void CheckableItemList::addItem(const QString &text, bool state) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags((item->flags() | Qt::ItemIsUserCheckable) & ~Qt::ItemIsSelectable);
	if (state) {
		item->setCheckState(Qt::Checked);
	}
	else {
		item->setCheckState(Qt::Unchecked);
	}
	itemSelectorList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	itemSelectorList->addItem(seperatorItem);
}

void CheckableItemList::checkStateChangedSlot(QListWidgetItem *item) {
	int index = itemSelectorList->row(item)/2;
	if (item->checkState() == Qt::Checked) {
		emit checkStateChanged(index, true);
	}
	else {
		emit checkStateChanged(index, false);
	}
}
