/*****************************************************************
 * File:			configurableitemlist.cpp
 * Created: 	01. August 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "labelwithtooltip.hpp"

LabelWithToolTip::LabelWithToolTip(QString name, QString toolTip, QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);
	QLabel *label = new QLabel(name);
	QWidget *spacer = new QWidget(this);
	if (toolTip != "") {
		QLabel *helpIcon = new QLabel(this);
		helpIcon->setPixmap((QIcon::fromTheme("info2")).pixmap(15,15));
		helpIcon->setToolTip(toolTip);
		helpIcon->setMaximumSize(10,40);
		layout->addWidget(helpIcon,0,1, Qt::AlignLeft);
		layout->addWidget(spacer,0,2);
	}
	layout->addWidget(label,0,0);
}
