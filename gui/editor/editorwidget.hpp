 /*****************************************************************
 	* File:			  editowidget.hpp
 	* Created: 	  23. October 2020
 	* Author:		  Timo Hueser
 	* Contact: 	  timo.hueser@gmail.com
 	* Copyright:  2022 Timo Hueser
 	* License:    LGPL v2.1
 	*****************************************************************/

#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "globals.hpp"
#include "keypointwidget.hpp"
#include "imageviewer.hpp"
#include "reprojectionwidget.hpp"
#include "datasetcontrolwidget.hpp"
#include "visualizationwindow.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QToolBar>
#include <QMouseEvent>
#include <QDrag>
#include <QFrame>
#include <QPushButton>
#include <QSplitter>
#include <QCheckBox>
#include <QTabWidget>
#include <QListWidget>


class EditorWidget : public QWidget {
	Q_OBJECT
	public:
		explicit EditorWidget(QWidget *parent = nullptr);

	signals:
		void datasetLoaded(QString selectedSegment);
		void cropToggled(bool toggle);
		void panToggled(bool toggle);
		void homeClicked();
		void quitClicked();
		void newSegmentLoaded();
		void frameChanged(int currentImgSetIndex, int currentFrameIndex);
		void imageTranformationChanged(int hueFactor, int saturationFactor, int brightnessFactor, int contrastFactor);
		void alwaysShowLabelsToggled(bool always_visible);
		void labelFontColorChanged(QColor color);
		void labelBackgroundColorChanged(QColor color);
		void keypointSizeChanged(int value);
		void keypointShapeChanged(const QString& entity, KeypointShape shape);
		void colorMapChanged(const QString& entity, ColorMap::ColorMapType type, QColor color);
		void minViewsChanged(int val);
		void errorThresholdChanged(float val);
		void boneLengthErrorThresholdChanged(float val);
		void brightnessChanged(int brightnessFactor);
		void cmdRPressed();  // toggle keypoint suppression
		void cmdEPressed();  // skip keypoint without any action

	public slots:
		void splitterMovedSlot(int pos, int index);
		void datasetLoadedSlot(bool isSetupAnnotation, QString selectedSegment);
		void frameChangedSlot(int index);
		void imgSetChangedSlot(int index);

	private:
		void keyPressEvent(QKeyEvent *e);
		QSplitter *mainSplitter;
		QSplitter *horizontalSplitter;

		QSplitter *leftSplitter;
		ReprojectionWidget *reprojectionWidget;
		DatasetControlWidget *datasetControlWidget;

		QWidget *imageViewerContainer;
		ImageViewer *imageViewer;

		VisualizationWindow *visualizationWindow;

		KeypointWidget *keypointWidget;
		QWidget  *buttonWidget;
		QPushButton *previousButton;
		QPushButton *nextButton;
		QPushButton *cropButton;
		QPushButton *panButton;
		QPushButton *homeButton;
		QPushButton *previousSetButton;
		QPushButton *nextSetButton;
		QPushButton *saveSetupButton;
		QPushButton *show3DButton;

		ImgSet *m_currentImgSet;
		int m_currentImgSetIndex;
		int m_currentFrameIndex;

		bool eventFilter(QObject *target, QEvent *event)
		{
			if (event->type() == QKeyEvent::KeyPress)
			{
				event->ignore();
				return true;
			}
			return QObject::eventFilter(target, event);
		}

	private slots:
		void previousClickedSlot();
		void nextClickedSlot();
		void cropToggledSlot(bool);
		void panToggledSlot(bool);
		void homeClickedSlot();
		void previousSetClickedSlot();
		void nextSetClickedSlot();
		void zoomFinishedSlot();
		void panFinishedSlot();
		void show3DClickedSlot();
};

#endif
