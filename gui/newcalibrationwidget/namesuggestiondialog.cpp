/*****************************************************************
	* File:			  namesuggestiondialog.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "namesuggestiondialog.hpp"

#include <QGridLayout>
#include <QGroupBox>


NameSuggestionDialog::NameSuggestionDialog(QString name, QList<QString> cameraNames,
			QWidget *parent) :
			QDialog(parent), m_name(name), m_cameraNames(cameraNames) {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	this->setWindowTitle("Detected " + name);
	this->resize(600,400);
	this->setMinimumSize(400,200);

	QGridLayout *layout = new QGridLayout(this);
	QLabel *descriptionLabel = new QLabel("Add the following " + name + "?");
	QGroupBox *listBox = new QGroupBox(name,this);
	QGridLayout *listboxlayout = new QGridLayout(listBox);
	namesList = new QListWidget();
	namesList->setFont(QFont("Sans Serif", 12));
	for (const auto & cameraName : cameraNames) {
		addItem(cameraName);
	}
	listboxlayout->addWidget(namesList,0,0);

	cancelButton = new QPushButton("Cancel",this);
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(40,40);
	connect(cancelButton, &QPushButton::clicked, this, &NameSuggestionDialog::reject);
	addButton = new QPushButton("Add", this);
	addButton->setIcon(QIcon::fromTheme("add"));
	addButton->setMinimumSize(40,40);
	connect(addButton, &QPushButton::clicked, this, &NameSuggestionDialog::accept);
	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spacer->setMaximumSize(10000,40);

	layout->addWidget(descriptionLabel,0,0,1,3);
	layout->addWidget(listBox,1,0,1,3);
	layout->addWidget(spacer,2,0);
	layout->addWidget(cancelButton,2,1);
	layout->addWidget(addButton,2,2);
}


void NameSuggestionDialog::addItem(const QString &text) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
	namesList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	namesList->addItem(seperatorItem);
}
