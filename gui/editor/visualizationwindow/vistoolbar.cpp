/*******************************************************************************
 * File:			  vistoolbar.cpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "vistoolbar.hpp"


VisualizationToolBar::VisualizationToolBar(QWidget *parent) : QToolBar(parent) {
	this->setFixedHeight(60);
	this->setIconSize(QSize(30, 30));

    cameraToggleButton = new QToolButton(this);
	cameraSetupAction = new QAction(this);
	createToolBarButton(cameraToggleButton, cameraSetupAction,
				QIcon::fromTheme("camera_rot"), true, true, QSize(50,50));
	this->addWidget(cameraToggleButton);
	cameraSetupAction->setChecked(false);
	connect(cameraSetupAction, &QAction::toggled,
			this, &VisualizationToolBar::toggleCameras);

	floorSetupButton = new QToolButton(this);
	floorToggleAction = new QAction(this);
	createToolBarButton(floorSetupButton, floorToggleAction,
				QIcon::fromTheme("checkerboard"), true, true, QSize(50,50));
	this->addWidget(floorSetupButton);
	floorToggleAction->setChecked(true);
	connect(floorToggleAction, &QAction::toggled,
			this, &VisualizationToolBar::toggleSetup);

	adjustViewToggleButton = new QToolButton(this);
	adjustViewToggleAction = new QAction(this);
	createToolBarButton(adjustViewToggleButton, adjustViewToggleAction,
				QIcon::fromTheme("resize"), true, true, QSize(50,50));
	this->addWidget(adjustViewToggleButton);
	adjustViewToggleAction->setChecked(true);
	connect(adjustViewToggleAction, &QAction::toggled,
			this, &VisualizationToolBar::toggleViewAdjust);

	QWidget *spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	this->addWidget(spacerWidget);

	saveButton = new QToolButton(this);
	saveAction = new QAction(this);
	createToolBarButton(saveButton, saveAction,
				QIcon::fromTheme("upload"), true, false, QSize(50,50));
	this->addWidget(saveButton);
	connect(saveAction, &QAction::triggered,
			this, &VisualizationToolBar::saveClicked);

	loadButton = new QToolButton(this);
	loadAction = new QAction(this);
	createToolBarButton(loadButton, loadAction,
				QIcon::fromTheme("download"), true, false, QSize(50,50));
	this->addWidget(loadButton);
	connect(loadAction, &QAction::triggered,
			this, &VisualizationToolBar::loadClicked);

}

