/*****************************************************************
	* File:			  pathwidget.hpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include "globals.hpp"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>


class DirPathWidget : public QWidget {
	Q_OBJECT

	public:
		explicit DirPathWidget(const QString& name, const QString& defaultPath = "",
					QWidget *parent = nullptr);
		void setPath(const QString& path) {pathEdit->setText(path);};
		QString path() {return pathEdit->text();};
		void setDefaultPath(const QString &path);
		void setPlaceholderText(const QString &text);

	signals:
		void pathChanged(const QString &path);

	private:
		QString m_name;
		QString m_currentPath;
		QLineEdit *pathEdit;
		QPushButton *pathButton;

	private slots:
		void pathEditedSlot();
		void pathChangedSlot(const QString& path);
		void pathClickedSlot();

};

#endif
