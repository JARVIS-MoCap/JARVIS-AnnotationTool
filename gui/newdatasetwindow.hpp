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


#include <QToolButton>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>

typedef struct DatasetConfig {
	QString datasetName = "New Dataset";
	QString dataType = "Videos";
	int numCameras= 12;
	int frameSetsRecording = 6;
	QString samplingMethod = "uniform";

} DatasetConfig;

class ConfigurableItemList : public QWidget {
	Q_OBJECT
	public:
		explicit ConfigurableItemList(QString name, DatasetConfig *datasetConfig, bool pathMode = false, QWidget *parent = nullptr);
		QListWidget *itemSelectorList;

	private:
		DatasetConfig	*m_datasetConfig;
		bool m_pathMode;
		QString m_name;
		QPushButton *moveItemUpButton;
		QPushButton *moveItemDownButton;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

		int getNumberSubfolders(QString path);
		bool isValidRecordingFolder(QString path);

	public slots:
		//void setNumberOfCamerasSlot(int num);

	private slots:
		void itemSelectedSlot(QListWidgetItem *item);
		void moveItemUpSlot();
		void moveItemDownSlot();
		void addItemSlot();
		void removeItemSlot();
};

class LabelWithToolTip : public QWidget {
	Q_OBJECT
	public:
		explicit LabelWithToolTip(QString name, QString toolTip, QWidget *parent = nullptr);
};

class NewDatasetWindow : public QWidget {
	Q_OBJECT
	public:
		explicit NewDatasetWindow(QWidget *parent = nullptr);

	public slots:

	signals:

	private:
		DatasetConfig	*m_datasetConfig;
		QSettings *settings;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		QLineEdit *datasetNameEdit;
		QRadioButton *videosButton;
		QRadioButton *imagesButton;
		QSpinBox *numCamerasBox;
		QSpinBox *frameSetsRecordingBox;
		QComboBox *samplingMethodCombo;

		ConfigurableItemList *recordingsItemList;
		ConfigurableItemList *entitiesItemList;
		ConfigurableItemList *keypointsItemList;

		QPushButton *loadButton;
		QPushButton *saveButton;
		QPushButton *createButton;


		private slots:
			void savePresetsClickedSlot();
			void loadPresetsClickedSlot();
			void savePresetSlot(const QString& preset);
			void loadPresetSlot(const QString& preset);
};



#endif
