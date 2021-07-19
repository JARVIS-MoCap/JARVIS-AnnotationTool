/*------------------------------------------------------------
 *  configurableitemlist.hpp
 *  Created: 08. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef CONFIGURABLEITEMLIST_H
#define CONFIGURABLEITEMLIST_H

#include "globals.hpp"
#include "dataset.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>

class ConfigurableItemList : public QWidget {
	Q_OBJECT
	public:
		explicit ConfigurableItemList(QString name, DatasetConfig *datasetConfig, QWidget *parent = nullptr);
		QListWidget *itemSelectorList;

		QList<QString> getItems();

	private:
		DatasetConfig	*m_datasetConfig;
		QString m_name;
		QPushButton *moveItemUpButton;
		QPushButton *moveItemDownButton;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

	public slots:
		//void setNumberOfCamerasSlot(int num);

	private slots:
		void itemSelectedSlot(QListWidgetItem *item);
		void moveItemUpSlot();
		void moveItemDownSlot();
		void addItemSlot();
		void removeItemSlot();
};


#endif
