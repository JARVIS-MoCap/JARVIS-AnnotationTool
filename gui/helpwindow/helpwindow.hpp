/*****************************************************************
	* File:			  helpwindow.hpp
	* Created: 	  08. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include "globals.hpp"

#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>
#include <QComboBox>
#include <QTextBrowser>
#include <QTextDocument>
#include <QSplitter>


class HelpWindow : public QWidget {
	Q_OBJECT
	public:
		explicit HelpWindow(QWidget *parent = nullptr);

	private:
		void setDocument(const QString &path);
		QTextBrowser *textBrowser;
		QTreeWidget *tableOfContents;
		QMap<QTreeWidgetItem*, QString> m_contentMap;
		QSplitter *mainSplitter;

		private slots:
			void itemSlectedSlot(QTreeWidgetItem *item, int column);
};


#endif
