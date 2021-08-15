/*****************************************************************
	* File:			  namesuggestiondialog.hpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef NAMESUGGESTIONDIALOG_H
#define NAMESUGGESTIONDIALOG_H

#include "globals.hpp"

#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QDialog>


class NameSuggestionDialog : public QDialog {
	Q_OBJECT

	public:
		explicit NameSuggestionDialog(QString name, QList<QString> cameraNames,
					QWidget *parent = nullptr);

	private:
		QString m_name;
		QList <QString> m_cameraNames;
		QListWidget *namesList;
		QPushButton *addButton;
		QPushButton *cancelButton;

		void addItem(const QString &text);
};

#endif
