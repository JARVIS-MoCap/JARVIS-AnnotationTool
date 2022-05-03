/*****************************************************************
	* File:			  configurableitemlist.hpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef CONFIGURABLEITEMLIST_H
#define CONFIGURABLEITEMLIST_H

#include "globals.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>


class ConfigurableItemList : public QWidget {
	Q_OBJECT

	public:
		explicit ConfigurableItemList(QString name, QWidget *parent = nullptr);
		QList<QString> getItems();
		void clear() {itemSelectorList->clear();};
		void addItem(const QString &item);

	signals:
		void itemsChanged(QList<QString> items);

	protected slots:
		void addItemSlot();

	private:
		QString m_name;
		QListWidget *itemSelectorList;
		QPushButton *moveItemUpButton;
		QPushButton *moveItemDownButton;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

	private slots:
		void itemSelectedSlot(QListWidgetItem *item);
		void moveItemUpSlot();
		void moveItemDownSlot();
		void removeItemSlot();
		void currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif
