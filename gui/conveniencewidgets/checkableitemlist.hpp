/*****************************************************************
	* File:			  checkableitemlist.hpp
	* Created: 	  12. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef CHECKABLEITEMLIST_H
#define CHECKABLEITEMLIST_H

#include "globals.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>


class CheckableItemList : public QWidget {
	Q_OBJECT

	public:
		explicit CheckableItemList(QString name, QWidget *parent = nullptr);
		QList<QPair<QString, bool>> getItems();
		void clear() {itemSelectorList->clear();};
		void addItem(const QString &item, bool state = true);

	signals:
		void itemsChanged(QList<QString> items);
		void checkStateChanged(int index, bool state);

	private:
		QListWidget *itemSelectorList;
		QString m_name;

	private slots:
		void checkStateChangedSlot(QListWidgetItem *item);
};

#endif
