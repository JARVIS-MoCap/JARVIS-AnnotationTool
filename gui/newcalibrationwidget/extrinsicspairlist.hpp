/*****************************************************************
	* File:			  extrinsicspairlist.hpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef EXTRINSICSPAIRLIST_H
#define EXTRINSICSPAIRLIST_H

#include "globals.hpp"
#include "labelwithtooltip.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QDialog>


class ExtrinsicsPairList : public QWidget {
	Q_OBJECT

	public:
		explicit ExtrinsicsPairList(QString name, QWidget *parent = nullptr);
		QListWidget *itemSelectorList;

		QList<QList<QString>> getItems();
		void setItems(QList<QList<QString>> items);

	public slots:
		void cameraNamesChangedSlot(QList<QString> cameraNames);

	private:
		QString m_name;
		QList <QString> m_cameraNames;
		QList<QList<QString>> m_cameraPairs;
		QPushButton *moveItemUpButton;
		QPushButton *moveItemDownButton;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

		void addItem(const QString &text);

	private slots:
		void moveItemUpSlot();
		void moveItemDownSlot();
		void addItemSlot();
		void removeItemSlot();
		void currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous);
		void itemDoubleClickedSlot(QListWidgetItem *item);
};


#endif
