/*****************************************************************
	* File:			  datasetlist.hpp
	* Created: 	  12. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef DATASETLIST_H
#define DATASETLIST_H

#include "globals.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>


class DatasetList : public QWidget {
	Q_OBJECT

	public:
		explicit DatasetList(QList<DatasetExportItem> &datasetExportItems,
					QList<QPair<QString,bool>> &entities,
					QList<QPair<QString,bool>> &keypoints,
					QList<SkeletonComponent> &skeleton, QWidget *parent = nullptr);
		void clear() {
			itemSelectorList->clear();
			m_datasetExportItems.clear();
		};
		void addItem(const QString &path);
		QList<QString> itemPaths();

	signals:
		void itemsChanged();

	public slots:
		void updateCountsSlot();

	private:
		QListWidget *itemSelectorList;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;
		QString m_currentPath;

		QList<DatasetExportItem> &m_datasetExportItems;
		QList<QPair<QString,bool>> &m_entities;
		QList<QPair<QString,bool>> &m_keypoints;
		QList<SkeletonComponent> &m_skeleton;


		bool analyseDatasetPath(DatasetExportItem * exportItem, const QString &basePath);
		bool getDatasetDirInfo(DatasetExportItem * exportItem, const QString &path);
		void addListItem(const QString &text);
		void updateCounts(DatasetExportItem &exportItem);

	private slots:
		void itemSelectedSlot(QListWidgetItem *item);
		void addItemSlot();
		void removeItemSlot();
		void currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif
