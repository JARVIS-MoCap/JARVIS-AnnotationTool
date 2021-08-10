/*------------------------------------------------------------
 *  extrinsicspairlist.hpp
 *  Created: 01. August 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

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

class PairCreatorWindow : public QDialog {
	Q_OBJECT
	public:
		explicit PairCreatorWindow(QList<QString> cameraNames, QWidget *parent = nullptr);
		QList<QString> getCameraPair();
		void setCameraPair(QList<QString> cameraPair);

	private:
		int m_mode = 0;
		QRadioButton *twoCamButton;
		QRadioButton *threeCamButton;

		QComboBox *primaryCombo;
		QLabel *intermediateLabel;
		QComboBox *intermediateCombo;
		QComboBox *secondaryCombo;

		QPushButton *addButtom;
		QPushButton *cancelButton;

	private slots:
		void modeToggledSlot(bool toggle);
		void cancelClickedSlot();
};


#endif
