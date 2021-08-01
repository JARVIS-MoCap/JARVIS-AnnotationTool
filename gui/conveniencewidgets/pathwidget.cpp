/*****************************************************************
 * File:			pathwidget.cpp
 * Created: 	01. August 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "pathwidget.hpp"

#include <QFileDialog>

DirPathWidget::DirPathWidget(const QString& name, const QString& defaultPath, QWidget *parent) :
	m_name(name),  QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);
	pathEdit = new QLineEdit(defaultPath, this);
	connect (pathEdit, &QLineEdit::textEdited, this, &DirPathWidget::pathChangedSlot);
	pathButton = new QPushButton();
	pathButton->setMinimumSize(25,25);
	pathButton->setMaximumSize(25,25);
	pathButton->setIcon(QIcon::fromTheme("folder"));
	connect(pathButton, &QPushButton::clicked, this, &DirPathWidget::pathClickedSlot);
	layout->addWidget(pathEdit,0,0);
	layout->addWidget(pathButton,0,1);
}

void DirPathWidget::pathChangedSlot(const QString& path) {
	emit pathChanged(path);
}

void DirPathWidget::pathClickedSlot() {
	QString dir = QFileDialog::getExistingDirectory(this,m_name, "./",
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir != "") {
		pathEdit->setText(dir);
	}
}
