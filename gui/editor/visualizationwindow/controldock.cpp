/*******************************************************************************
 * File:			  controldock.cpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "controldock.hpp"
#include "spoiler.hpp"

#include <QGroupBox>


ControlDock::ControlDock(QWidget *parent) : QDockWidget(parent) {
	mainWidget = new QWidget(this);
	this->setWidget(mainWidget);
	QGridLayout *layout = new QGridLayout(mainWidget);
	QToolBar *newBar = new QToolBar(this);
	this->setTitleBarWidget(newBar);

    cameraButtonWidget = new QWidget(mainWidget);
	camerabuttonlayout = new QGridLayout(cameraButtonWidget);
	camerabuttonlayout->setSpacing(2);

	Spoiler *displaySettingsWidget = new Spoiler("Settings", 300, mainWidget);
	QGridLayout *displaysettingslayout = new QGridLayout();
	QLabel *keypointSizeLabel = new QLabel("Keypoint Size");
	keypointSizeBox = new QSpinBox(mainWidget);
	keypointSizeBox->setValue(5);
	keypointSizeBox->setRange(0,100);
	connect(keypointSizeBox, QOverload<int>::of(&QSpinBox::valueChanged),
				this, &ControlDock::keypointRadiusChanged);
	QLabel *skeletonSizeLabel = new QLabel("Skeleton Thickness");
	skeletonSizeBox = new QSpinBox(mainWidget);
	skeletonSizeBox->setValue(1);
	skeletonSizeBox->setRange(0,100);
	connect(skeletonSizeBox, QOverload<int>::of(&QSpinBox::valueChanged),
				this, &ControlDock::skeletonThicknessChanged);

	displaysettingslayout->addWidget(keypointSizeLabel,0,0);
	displaysettingslayout->addWidget(keypointSizeBox,0,1);
	displaysettingslayout->addWidget(skeletonSizeLabel,1,0);
	displaysettingslayout->addWidget(skeletonSizeBox,1,1);
	displaySettingsWidget->setContentLayout(*displaysettingslayout);


	layout->addWidget(cameraButtonWidget, 0,0,1,2);
	layout->addWidget(displaySettingsWidget, 1,0,1,2);
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
