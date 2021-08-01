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

	private:
		QSettings *settings;
		QErrorMessage *m_errorMsg;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		QPushButton *loadButton;
		QPushButton *saveButton;
		QPushButton *calibrateButton;

		YesNoRadioWidget *seperateRadioWidget;
		YesNoRadioWidget *calibrateIntrinsicsRadioWidget;
		YesNoRadioWidget *calibrateExtrinsicsRadioWidget;
		DirPathWidget *intrinsicsPathWidget;
		DirPathWidget *extrinsicsPathWidget;
		QSpinBox *intrinsicsFramesEdit;
		QSpinBox *extrinsicsFramesEdit;
		QSpinBox *widthEdit;
		QSpinBox *heightEdit;
		QDoubleSpinBox *sideLengthEdit;

		ConfigurableItemList *cameraList;

		ExtrinsicsPairList *extrinsicsPairList;


	private slots:
		void savePresetsClickedSlot();
		void loadPresetsClickedSlot();
		void savePresetSlot(const QString& preset);
		void loadPresetSlot(const QString& preset);

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
