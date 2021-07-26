/*------------------------------------------------------------
 *  editowidget.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "globals.hpp"
#include "keypointwidget.hpp"
#include "imageviewer.hpp"
#include "loaddatasetwindow.hpp"
#include "newdatasetwindow.hpp"
#include "reprojectionwidget.hpp"
#include "datasetcontrolwidget.hpp"

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
#include <QStackedWidget>



class EditorWidget : public QWidget {
	Q_OBJECT
	public:
		explicit EditorWidget(QWidget *parent = nullptr);

	public slots:
		void splitterMovedSlot(int pos, int index);
		void datasetLoadedSlot();
		void frameChangedSlot(int index);
		void imgSetChangedSlot(int index);
		void exitToMainPageSlot();

	signals:
		void zoomToggled(bool toggle);
		void panToggled(bool toggle);
		void homeClicked();
		void quitClicked();
		void datasetLoaded();
		void frameChanged(int currentImgSetIndex, int currentFrameIndex);
		void imageTranformationChanged(int hueFactor, int saturationFactor, int brightnessFactor);
		void keypointSizeChanged(int value);
		void keypointShapeChanged(const QString& entity, KeypointShape shape);
		void colorMapChanged(const QString& entity, ColorMap::ColorMapType type, QColor color);

	private:
		QSplitter *mainSplitter;
		QSplitter *horizontalSplitter;
		QStackedWidget *stackedWidget;

		QSplitter *leftSplitter;
		ReprojectionWidget *reprojectionWidget;
		DatasetControlWidget *datasetControlWidget;

		QWidget *datasetWidget;
		QPushButton *loadDatasetButton;
		QPushButton *newDatasetButton;
		LoadDatasetWindow *loadDatasetWindow;
		NewDatasetWindow *newDatasetWindow;

		QWidget *imageViewerContainer;
		ImageViewer *imageViewer;


		KeypointWidget *keypointWidget;
		QWidget  *buttonWidget;
		QPushButton *previousButton;
		QPushButton *nextButton;
		QPushButton *zoomButton;
		QPushButton *panButton;
		QPushButton *homeButton;
		QPushButton *previousSetButton;
		QPushButton *nextSetButton;
		QPushButton *quitButton;

		ImgSet *m_currentImgSet;
		int m_currentImgSetIndex;
		int m_currentFrameIndex;

	private slots:
		void previousClickedSlot();
		void nextClickedSlot();
		void zoomToggledSlot(bool);
		void panToggledSlot(bool);
		void homeClickedSlot();
		void previousSetClickedSlot();
		void nextSetClickedSlot();
		void zoomFinishedSlot();
		void panFinishedSlot();
		void loadDatasetClickedSlot();
		void newDatasetClickedSlot();
		void quitClickedSlot();
};

#endif
