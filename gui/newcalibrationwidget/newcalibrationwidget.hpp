/*****************************************************************
	* File:			  newcalibrationwidget.hpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef NEWCALIBRATIONWIDGET_H
#define NEWCALIBRATIONWIDGET_H

#include "globals.hpp"
#include "presetswindow.hpp"
#include "labelwithtooltip.hpp"
#include "pathwidget.hpp"
#include "configurableitemlist.hpp"
#include "extrinsicspairlist.hpp"
#include "calibrationtool.hpp"
#include "calibrationprogressinfowindow.hpp"
#include "calibrationstatisticswindow.hpp"
#include "namesuggestiondialog.hpp"
#include "yesnoradiowidget.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QErrorMessage>
#include <QRadioButton>


class NewCalibrationWidget : public QWidget {
	Q_OBJECT

	public:
		explicit NewCalibrationWidget(QWidget *parent = nullptr);

	signals:
		void makeCalibrationSet();

	private:
		QSettings *settings;
		QErrorMessage *m_errorMsg;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		CalibrationTool *calibrationTool;
		CalibrationProgressInfoWindow *calibrationProgressInfoWindow;
		CalibrationConfig *m_calibrationConfig;
		QList<QString> m_validRecordingFormats = {"avi", "mp4", "mov", "wmv"};

		QLineEdit *calibrationSetNameEdit;
		DirPathWidget *calibrationSetPathWidget;
		YesNoRadioWidget *seperateRadioWidget;
		YesNoRadioWidget *calibrateExtrinsicsRadioWidget;
		DirPathWidget *intrinsicsPathWidget;
		DirPathWidget *extrinsicsPathWidget;
		QPushButton *updateNamesListButton;

		QSpinBox *maxSamplingFrameRateEdit;
		QSpinBox *intrinsicsFramesEdit;
		QSpinBox *extrinsicsFramesEdit;
		YesNoRadioWidget *saveDebugRadioWidget;

		QComboBox *boardTypeCombo;
		QSpinBox *widthEdit;
		QSpinBox *heightEdit;
		QDoubleSpinBox *sideLengthEdit;
		QGroupBox *checkerBoardPreviewBox;
		QLabel *checkerBoardPreview;
		QLabel *checkerBoardPreviewLabel;

		ConfigurableItemList *cameraList;

		ExtrinsicsPairList *extrinsicsPairList;

		QPushButton *loadButton;
		QPushButton *saveButton;
		QPushButton *calibrateButton;

		bool checkIntrinsics(const QString& path, QString & errorMsg);
		bool checkExtrinsics(const QString& path, QString & errorMsg);
		bool checkCheckerboard();
		bool checkCalibrationExists(const QString& path);
		bool checkIsValidRecording(const QString& path, const QString& recording);
		QImage createCheckerboardPreview();


	private slots:
		void savePresetsClickedSlot();
		void loadPresetsClickedSlot();
		void savePresetSlot(const QString& preset);
		void loadPresetSlot(const QString& preset);

		void updateNamesListSlot();
		void calibrateClickedSlot();
		void calibrationFinishedSlot();
		void calibrationCanceledSlot();
		void calibrationErrorSlot(const QString &errorMsg);
		void sperateRadioStateChangedSlot(bool state);
		void calibrateExtrinsicsRadioStateChangedSlot(bool state);
		void checkerBoardPatternChangesSlot(int val);

		void intrinsicsPathChangedSlot(const QString &path);
};

#endif
