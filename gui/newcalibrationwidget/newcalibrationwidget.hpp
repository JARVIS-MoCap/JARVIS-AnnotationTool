/*****************************************************************
	* File:			  newcalibrationwidget.hpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#ifndef NEWCALIBRATIONWIDGET_H
#define NEWCALIBRATIONWIDGET_H

#include "globals.hpp"
#include "presetswindow.hpp"
#include "labelwithtooltip.hpp"
#include "pathwidget.hpp"
#include "configurableitemlist.hpp"
#include "cameranameslist.hpp"
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
		DirPathWidget *intrinsicsPathWidget;
		DirPathWidget *extrinsicsPathWidget;
		QPushButton *updateNamesListButton;

		QSpinBox *intrinsicsFramesEdit;
		QSpinBox *extrinsicsFramesEdit;
		YesNoRadioWidget *saveDebugRadioWidget;

		QComboBox *boardTypeCombo;
		LabelWithToolTip *charucoPatternLabel;
		QComboBox *charucoPatternCombo;

		QSpinBox *widthEdit;
		QSpinBox *heightEdit;
		QDoubleSpinBox *sideLengthEdit;
		LabelWithToolTip *markerLengthLabel;
		QDoubleSpinBox *markerLengthEdit;
		QGroupBox *checkerBoardPreviewBox;
		QLabel *checkerBoardPreview;
		QLabel *checkerBoardPreviewLabel;

		CameraNamesList *cameraList;

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
		void checkerBoardPatternChangesSlot(int val);
		void boardTypeChangedSlot(int val);

		void intrinsicsPathChangedSlot(const QString &path);
};

#endif
