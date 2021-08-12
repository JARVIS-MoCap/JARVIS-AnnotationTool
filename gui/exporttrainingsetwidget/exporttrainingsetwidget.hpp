/*------------------------------------------------------------
 *  exporttrainingsetwidget.hpp
 *  Created: 30. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef EXPORTTRAININGSETWIDGET_H
#define EXPORTTRAININGSETWIDGET_H

#include "globals.hpp"
#include "presetswindow.hpp"
#include "labelwithtooltip.hpp"
#include "pathwidget.hpp"
#include "yesnoradiowidget.hpp"
#include "compositionchartview.hpp"

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

class ExportTrainingsetWidget : public QWidget {
	Q_OBJECT
	public:
		explicit ExportTrainingsetWidget(QWidget *parent = nullptr);

	signals:

	public slots:


	private:
		QSettings *settings;
		QErrorMessage *m_errorMsg;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		QLineEdit *trainingsetNameEdit;
		DirPathWidget *trainingsetSavePathWidget;
		QRadioButton *type2DButton;
		QRadioButton *type3DButton;
		DirPathWidget *intrinsicsPathWidget;
		DirPathWidget *extrinsicsPathWidget;
		QDoubleSpinBox *validationFractionEdit;
		YesNoRadioWidget *shuffleBeforeSplitWidget;
		QSpinBox *shuffleSeedEdit;
		QLineEdit *numberFramesTrainingEdit;
		QLineEdit *numberFramesValidationEdit;
		QLineEdit *numberEntitiesEdit;
		QLineEdit *numberKeypointsEdit;
		CompositionChartView *compositionChartView;


		QPushButton *saveButton;
		QPushButton *loadButton;
		QPushButton *exportButton;

		private slots:
		void savePresetsClickedSlot();
		void loadPresetsClickedSlot();
		void savePresetSlot(const QString& preset);
		void loadPresetSlot(const QString& preset);

};


#endif
