/*****************************************************************
	* File:			  cameranameslist.hpp
	* Created: 	  16. February 2022
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef EXTRINSICSPAIRLIST_H
#define EXTRINSICSPAIRLIST_H

#include "globals.hpp"
#include "labelwithtooltip.hpp"
#include "configurableitemlist.hpp"


#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QDialog>


class CameraNamesList : public ConfigurableItemList {
	Q_OBJECT

	public:
		explicit CameraNamesList(QString name, QWidget *parent = nullptr);

	public slots:

	private:

	private slots:
};


#endif
