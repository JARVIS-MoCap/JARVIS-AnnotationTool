/*****************************************************************
	* File:			  skeletontablewidget.hpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef SKELETONTABLEWIDGET_H
#define SKELETONTABLEWIDGET_H

#include "globals.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QTableWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QDialog>


class SkeletonTableWidget : public QWidget {
	Q_OBJECT

	public:
		explicit SkeletonTableWidget(QString name, QWidget *parent = nullptr);
		QTableWidget *skeletonTable;

		QList<SkeletonComponent> getItems();
		void setItems(QList<SkeletonComponent> items);

	public slots:
		void setKeypointsListSlot(QList<QString> keypointsList);
	private:
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

		QString m_name;
		QList<QString> m_keypointList;

		void updateComboBoxes();

	private slots:
		void addItemSlot();
		void removeItemSlot();
		void currentItemChangedSlot(QTableWidgetItem *current, QTableWidgetItem *previous);
		void cellDoubleClickedSlot(int row, int column);
};


#endif
