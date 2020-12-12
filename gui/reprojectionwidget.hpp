/*------------------------------------------------------------
 *  reprojectionwidget.hpp
 *  Created: 03. December 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef REPROJECTIONWIDGET_H
#define REPROJECTIONWIDGET_H

#include "globals.hpp"
#include "dataset.hpp"
#include "reprojectiontool.hpp"
#include "switch.hpp"
#include "colormap.hpp"
#include "reprojectionchartwidget.hpp"


#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>


class ReprojectionWidget : public QWidget {
	Q_OBJECT
	public:
		explicit ReprojectionWidget(QWidget *parent = nullptr);


	public slots:
		void datasetLoadedSlot();
		void calculateReprojectionSlot(int currentImgSetIndex, int currentFrameIndex);

	signals:
		void reprojectedPoints(ImgSet *imgSet, int frameIndex);
		void reprojectionToolToggled(bool toggle);
		void datasetLoaded();
		void errorThresholdChanged(double value);
		void reprojectionErrorsUpdated(QMap<QString, std::vector<double> *>);


	private:
		void checkIntrinsicPathsAdded();
		void checkExtrinsicPathsAdded();
		void savePaths();
		void loadPaths(bool onlyExtrinsics = false);
		void undoReprojection();

		ReprojectionChartWidget *reprojectionChartWidget;

		QSettings *settings;
		ColorMap *m_colorMap;
		ReprojectionTool *reprojectionTool = nullptr;

		Switch *toggleSwitch;
		QStackedWidget *stackedWidget;

		QWidget *intrinsicsSetup;
		QGridLayout *intrinsicssetuplayout;
		QGroupBox *intrinsicsBox;
		QGridLayout *intrinsicslayout;
		QPushButton *initIntrinsicsButton;
		QWidget *extrinsicsSetup;
		QGridLayout *extrinsicssetuplayout;
		QGroupBox *extrinsicsBox;
		QGridLayout*extrinsicslayout;
		QComboBox *primaryCombo;
		QPushButton *initReprojectionButton;

		QWidget *reprojectionController;
		QGridLayout *reprojectioncontrollerlayout;
		QGroupBox *reprojectionSettingsBox;
		QSpinBox *minViewsEdit;
		QDoubleSpinBox *errorThresholdEdit;

		QList< QLineEdit* > intrinsicsPathEdits;
		//QList< QPushButton* > intrinsicsPathButtons;
		QList< QLineEdit* > extrinsicsPathEdits;
		bool m_reprojectionActive = false;
		int m_numCameras = 0;
		int m_primaryIndex = 0;
		int m_minViews = 2;
		double m_errorThreshold = 10.0;
		QList<QString> m_entitiesList;
		QList<QString> m_bodypartsList;
		int m_currentImgSetIndex  = 0;
		int m_currentFrameIndex = 0;
		QMap<QString, std::vector<double> *> m_reprojectionErrors;

	private slots:
		void switchToggledSlot(bool toggle);
		void primaryChangedSlot(QString);
		void intrinsicsPathClickedSlot();
		void extrinsicsPathClickedSlot();
		void initIntrinsicsClickedSlot();
		void initReprojectionClickedSlot();
		void minViewsChangedSlot(int value);
		void errorThresholdChangedSlot(double value);



};

#endif
