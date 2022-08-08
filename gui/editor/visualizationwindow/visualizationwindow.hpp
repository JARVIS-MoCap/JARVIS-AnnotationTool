/*******************************************************************************
 * File:			  visualizationwindow.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef VISUALIZATIONWINDOW_H
#define VISUALIZATIONWINDOW_H

#include "globals.hpp"
#include "colormap.hpp"
#include "reprojectiontool.hpp"
#include "vistoolbar.hpp"
#include "controldock.hpp"
#include "viewport.hpp"

#include <opencv2/core.hpp>

#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QGroupBox>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QMainWindow>



class VisualizationWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit VisualizationWindow(QMainWindow *parent = nullptr);

		void opened();

	signals:

	public slots:
		void update3DCoordsSlot(QMap<QString, QVector3D> coords3D);
		void reprojectionToolUpdatedSlot(ReprojectionTool *reproTool);
		void saveClickedSlot();

	private:
		QSettings *settings;

		QWidget *mainWidget;
		ViewPort *viewPort;
		VisualizationToolBar *visToolBar;
		ControlDock *controlDock;

		int m_camMoveCounter = 0;

		ReprojectionTool *m_reproTool = nullptr;

		void drawCameras();

		private slots: 
			void loadClickedSlot();
			void cameraMovedSlot();
			void cameraViewChangedSlot(int index);
			void viewInitSlot();

};

#endif
