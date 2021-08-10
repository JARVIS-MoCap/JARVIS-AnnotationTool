/*------------------------------------------------------------
 *  newdatasetwindow.hpp
 *  Created: 07. July 2020
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef NEWDATASETWINDOW_H
#define NEWDATASETWINDOW_H

#include "globals.hpp"
#include "dataset.hpp"
#include "presetswindow.hpp"
#include "configurableitemlist.hpp"
#include "recordingstable.hpp"
#include "datasetcreator.hpp"
#include "datasetprogressinfowindow.hpp"
#include "labelwithtooltip.hpp"
#include "pathwidget.hpp"



#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>
#include <QErrorMessage>


class NewDatasetWindow : public QWidget {
	Q_OBJECT
	public:
		explicit NewDatasetWindow(QWidget *parent = nullptr);

	public slots:

	signals:
		void createDataset(QList<RecordingItem> recordings, QList<QString> entities, QList<QString> keypoints);

	private:
		DatasetConfig	*m_datasetConfig;
		DatasetCreator *datasetCreator;
		DatasetProgressInfoWindow *datasetProgressInfoWindow;
		QSettings *settings;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		QLineEdit *datasetNameEdit;
		DirPathWidget *datasetPathWidget;
		QRadioButton *videosButton;
		QRadioButton *imagesButton;
		QSpinBox *frameSetsRecordingBox;
		QComboBox *samplingMethodCombo;

		RecordingsTable *recordingsTable;
		ConfigurableItemList *entitiesItemList;
		ConfigurableItemList *keypointsItemList;


		QPushButton *loadButton;
		QPushButton *saveButton;
		QPushButton *createButton;

		QErrorMessage *m_errorMsg;

		bool checkDatasetExists(const QString &path);



		private slots:
			void createDatasetClickedSlot();
			void datasetCreatedSot();
			void datasetCreationFailedSlot(QString errorMsg);

			void savePresetsClickedSlot();
			void loadPresetsClickedSlot();
			void savePresetSlot(const QString& preset);
			void loadPresetSlot(const QString& preset);

};


#endif
