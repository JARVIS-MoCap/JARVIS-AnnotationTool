/*------------------------------------------------------------
 *  helpwindow.hpp
 *  Created: 	08. August 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

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

	public slots:

	signals:

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
