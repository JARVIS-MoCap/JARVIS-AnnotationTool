/*****************************************************************
	* File:			  labelwithtooltip.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "labelwithtooltip.hpp"


LabelWithToolTip::LabelWithToolTip(QString name, QString toolTip, QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(0,0,0,0);
	QLabel *label = new QLabel(name);
	QWidget *spacer = new QWidget(this);
	spacer->setMinimumSize(20,0);
	label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	int i = 1;
	if (toolTip != "") {
		QLabel *helpIcon = new QLabel(this);
		helpIcon->setPixmap((QIcon::fromTheme("info2")).pixmap(15,15));
		helpIcon->setToolTip(toolTip);
		helpIcon->setMaximumSize(10,40);
		layout->addWidget(helpIcon,0,i++, Qt::AlignLeft);
	}
	layout->addWidget(label,0,0);
	layout->addWidget(spacer,0,i);
}
