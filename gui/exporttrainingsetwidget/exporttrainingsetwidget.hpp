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
#include "annotationstatisticschartview.hpp"
#include "datasetlist.hpp"
#include "checkableitemlist.hpp"
#include "trainingsetexporter.hpp"


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
		void updateCounts();
		void exportTrainingset();

	public slots:


	private:
		QSettings *settings;
		QErrorMessage *m_errorMsg;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;
		TrainingSetExporter *trainingSetExporter;

		QLineEdit *trainingsetNameEdit;
		DirPathWidget *trainingsetSavePathWidget;
		QRadioButton *type2DButton;
		QRadioButton *type3DButton;
		DirPathWidget *intrinsicsPathWidget;
		DirPathWidget *extrinsicsPathWidget;
		QDoubleSpinBox *validationFractionEdit;
		YesNoRadioWidget *shuffleBeforeSplitWidget;
		YesNoRadioWidget *randomShuffleSeedWidget;
		QSpinBox *shuffleSeedEdit;
		QLineEdit *numberFramesTrainingEdit;
		QLineEdit *numberFramesValidationEdit;

		CompositionChartView *compositionChartView;
		QLabel *annotationStatisticsLabel;
		QGroupBox *annotationStatisticsBox;
		AnnotationStatisticsChartView *annotationChartView;

		DatasetList *datasetList;
		CheckableItemList *entitiesList;
		CheckableItemList *keypointsList;

		QPushButton *saveButton;
		QPushButton *loadButton;
		QPushButton *exportButton;

		QList<DatasetExportItem> m_datasetExportItems;
		QList<AnnotationCount> m_annotationCounts;
		QList<QPair<QString,bool>> m_entities;
		QList<QPair<QString,bool>> m_keypoints;

	private slots:
		void hoverStartedSlot(int index);
		void hoverEndedSlot();
		void trainingsetTypeChangedSlot(bool toggle);
		void shuffleBeforeSplitStateChangedSlot(bool state);
		void randomShuffleSeedStateChangedSlot(bool state);
		void datasetListChangedSlot();
		void entitiesListChangedSlot(int row, bool state);
		void keypointsListChangedSlot(int row, bool state);
		void exportClickedSlot();


		void savePresetsClickedSlot();
		void loadPresetsClickedSlot();
		void savePresetSlot(const QString& preset);
		void loadPresetSlot(const QString& preset);

};


#endif
