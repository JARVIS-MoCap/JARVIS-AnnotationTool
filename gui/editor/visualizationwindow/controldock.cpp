/*******************************************************************************
 * File:			  controldock.cpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "controldock.hpp"


ControlDock::ControlDock(QWidget *parent) : QDockWidget(parent) {
	mainWidget = new QWidget(this);
	this->setWidget(mainWidget);
	QGridLayout *layout = new QGridLayout(mainWidget);
	QToolBar *newBar = new QToolBar(this);
	this->setTitleBarWidget(newBar);

    cameraButtonWidget = new QWidget();
	camerabuttonlayout = new QGridLayout(cameraButtonWidget);
	camerabuttonlayout->setSpacing(2);

	layout->addWidget(cameraButtonWidget, 0,0,1,2);
}

void ControlDock::updateCameraButtons(const QList<QString> cameraNames) {
for (const auto & button : cameraButtonArray) {
		delete button;
	}
	cameraButtonArray.clear();
	int i = 0;
	for (const auto & name : cameraNames) {
		QPushButton *button = new QPushButton(name);
		button->setCheckable(true);
		button->setStyleSheet("QPushButton {background-color: palette(base);border: 0px;}"
							  "QPushButton:checked { background-color: palette(highlight)}"
							  "QPushButton:hover { background-color: rgb(32,100,164)}");
		cameraButtonArray.append(button);
		button->setMinimumSize(80,30);
		button->setMaximumSize(200,40);
		connect(button, &QPushButton::clicked, this, &ControlDock::cameraViewChangedSlot);
		camerabuttonlayout->addWidget(button, i/4,i%4);
		i++;
	}
}

void ControlDock::resetCameraButtons() {
    for (const auto & b : cameraButtonArray) {
		b->setChecked(false);
	}
}


void ControlDock::cameraViewChangedSlot() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
	int index = cameraButtonArray.indexOf(button);
    resetCameraButtons();
	button->setChecked(true);
    emit cameraViewChanged(index);
}