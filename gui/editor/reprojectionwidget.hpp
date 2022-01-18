/*****************************************************************
	* File:			  reprojectionwidget.hpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef REPROJECTIONWIDGET_H
#define REPROJECTIONWIDGET_H

#include "globals.hpp"
#include "dataset.hpp"
#include "reprojectiontool.hpp"
#include "switch.hpp"
#include "colormap.hpp"
#include "reprojectionchartwidget.hpp"
#include "bonelengthchartwidget.hpp"


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

	signals:
		void reprojectedPoints(ImgSet *imgSet, int frameIndex);
		void reprojectionToolToggled(bool toggle);
		void datasetLoaded();
		void errorThresholdChanged(double value);
		void boneLengthErrorThresholdChanged(double value);
		void reprojectionErrorsUpdated(QMap<QString, std::vector<double> *>);

	public slots:
		void datasetLoadedSlot();
		void calculateReprojectionSlot(int currentImgSetIndex, int currentFrameIndex);
		void minViewsChangedSlot(int value);
		// void errorThresholdChangedSlot(double value);
		// void boneLengthErrorThresholdChangedSlot(double value);

	private:
		bool checkCalibParams(const QString &path);
		void undoReprojection();
		void calculateAllReprojections();
		void getSettings();

		ReprojectionChartWidget *reprojectionChartWidget;
		BoneLengthChartWidget *boneLengthChartWidget;

		QSettings *settings;
		ColorMap *m_colorMap;
		ReprojectionTool *reprojectionTool = nullptr;

		Switch *toggleSwitch;
		QLabel *modeLabel;
		QComboBox *modeCombo;
		QStackedWidget *stackedWidget;

		QWidget *calibrationSetup;
		QPushButton *initReprojectionButton;

		QWidget *reprojectionController;
		QGridLayout *reprojectioncontrollerlayout;

		bool m_reprojectionActive = false;
		bool m_calibExists = false;
		int m_numCameras = 0;
		int m_minViews = 2;
		double m_errorThreshold = 10.0;
		QList<QString> m_entitiesList;
		QList<QString> m_bodypartsList;
		int m_currentImgSetIndex  = 0;
		int m_currentFrameIndex = 0;
		QMap<QString, std::vector<double> *> m_reprojectionErrors;
		QMap<QString, std::vector<double> *> m_boneLengthErrors;


		QDir m_parameterDir;

	private slots:
		void switchToggledSlot(bool toggle);
		void initReprojectionClickedSlot();
		void modeComboChangedSlot(const QString& mode);
};

#endif
