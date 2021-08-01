/*------------------------------------------------------------
 *  pathwidget.hpp
 *  Created: 	01. August 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

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
		explicit DirPathWidget(const QString& name, const QString& defaultPath = "", QWidget *parent = nullptr);
		void setPath(const QString& path) {pathEdit->setText(path);};
		QString path() {return pathEdit->text();};

	signals:
		void pathChanged(const QString &path);

	private:
		QString m_name;
		QLineEdit *pathEdit;
		QPushButton *pathButton;

	private slots:
		void pathChangedSlot(const QString& path);
		void pathClickedSlot();

};

#endif
