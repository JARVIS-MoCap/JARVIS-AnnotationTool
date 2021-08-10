/*------------------------------------------------------------
 *  newcalibrationwidget.hpp
 *  Created: 30. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

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

class YesNoRadioWidget;

class NewCalibrationWidget : public QWidget {
	Q_OBJECT
	public:
		explicit NewCalibrationWidget(QWidget *parent = nullptr);

	public slots:

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
		QSpinBox *widthEdit;
		QSpinBox *heightEdit;
		QDoubleSpinBox *sideLengthEdit;

		ConfigurableItemList *cameraList;

		ExtrinsicsPairList *extrinsicsPairList;

		QPushButton *loadButton;
		QPushButton *saveButton;
		QPushButton *calibrateButton;

		bool checkIntrinsics(const QString& path);
		bool checkExtrinsics(const QString& path);
		bool checkCheckerboard();
		bool checkCalibrationExists(const QString& path);
		bool checkIsValidRecording(const QString& path, const QString& recording);


	private slots:
		void savePresetsClickedSlot();
		void loadPresetsClickedSlot();
		void savePresetSlot(const QString& preset);
		void loadPresetSlot(const QString& preset);

		void updateNamesListSlot();
		void calibrateClickedSlot();
		void calibrationFinishedSlot();
		void calibrationCanceledSlot();
		void sperateRadioStateChangedSlot(bool state);
		void calibrateExtrinsicsRadioStateChangedSlot(bool state);

		void intrinsicsPathChangedSlot(const QString &path);

};


class YesNoRadioWidget : public QWidget {
	Q_OBJECT
	public:
			explicit YesNoRadioWidget(QWidget *parent = nullptr);
			void setState(bool state);
			bool state() {return yesButton->isChecked();};

	signals:
		void stateChanged(bool state);

	private:
		QRadioButton *yesButton;
		QRadioButton *noButton;
};


#endif
