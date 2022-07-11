/*******************************************************************************
 * File:		vistoolbar.hpp
 * Created: 	  07. July 2022
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef VISTOOLBAR_H
#define VISTOOLBAR_H

#include "globals.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QToolBar>



class VisualizationToolBar : public QToolBar {
	Q_OBJECT

	public:
		explicit VisualizationToolBar(QWidget *parent = nullptr);

	signals:
		void toggleCameras(bool toggle);
		void toggleSetup(bool toggle);
		void toggleViewAdjust(bool toggle);
		void saveClicked();
		void loadClicked();

	private:
		QToolButton *cameraToggleButton;
		QAction *cameraSetupAction;
		QToolButton *floorSetupButton;
		QAction *floorToggleAction;
		QToolButton *adjustViewToggleButton;
		QAction *adjustViewToggleAction;
		QToolButton *saveButton;
		QAction *saveAction;
		QToolButton *loadButton;
		QAction *loadAction;

	private slots:
};

#endif
