/*****************************************************************
	* File:			  pathwidget.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "pathwidget.hpp"

#include <QFileDialog>


DirPathWidget::DirPathWidget(const QString& name, const QString& defaultPath, QWidget *parent) :
	QWidget(parent), m_name(name) {
	if (defaultPath != "") {
		m_currentPath = defaultPath;
	}
	else {
		m_currentPath = QDir::homePath();
	}
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);
	pathEdit = new QLineEdit(defaultPath, this);
	connect (pathEdit, &QLineEdit::returnPressed, this, &DirPathWidget::pathEditedSlot);
	connect (pathEdit, &QLineEdit::textChanged, this, &DirPathWidget::pathChangedSlot);
	pathButton = new QPushButton();
	pathButton->setMinimumSize(25,25);
	pathButton->setMaximumSize(25,25);
	pathButton->setIcon(QIcon::fromTheme("folder"));
	connect(pathButton, &QPushButton::clicked, this, &DirPathWidget::pathClickedSlot);
	layout->addWidget(pathEdit,0,0);
	layout->addWidget(pathButton,0,1);
}


void DirPathWidget::pathEditedSlot() {
	QString path = pathEdit->text();
	if (path == "") {
		m_currentPath	= QDir::homePath();
	}
	else {
		QDir dir = QDir(path);
		dir.cdUp();
		m_currentPath = dir.path();
	}
	emit pathChanged(path);
}


void DirPathWidget::pathChangedSlot(const QString& path) {
	if (path == "") {
		m_currentPath	= QDir::homePath();
	}
	else {
		QDir dir = QDir(path);
		dir.cdUp();
		m_currentPath = dir.path();
	}
}


void DirPathWidget::pathClickedSlot() {
	QString dir = QFileDialog::getExistingDirectory(this,m_name, m_currentPath,
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "" && pathEdit->text() != dir) {
		pathEdit->setText(dir);
		emit pathChanged(dir);
	}
}


void DirPathWidget::setDefaultPath(const QString &path) {
	m_currentPath = path;
}

void DirPathWidget::setPlaceholderText(const QString &text) {
	pathEdit->setPlaceholderText(text);
}
