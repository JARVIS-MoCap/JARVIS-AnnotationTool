/*****************************************************************
	* File:			  cameranameslist.hpp
	* Created: 	  16. February 2022
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef CAMERANAMESLIST_H
#define CAMERANAMESLIST_H

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


class CameraNamesList : public QWidget {
	Q_OBJECT

	public:
		explicit CameraNamesList(QString name, QWidget *parent = nullptr);
		QList<QString> getItems();
		void clear() {itemSelectorList->clear();};
		void addItem(const QString &item);
		void addCameras(QList<QString> cameras);

	signals:
		void itemsChanged(QList<QString> items);

	protected slots:
		void addItemSlot();

	private:
		QList<QString> m_cameras;
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
