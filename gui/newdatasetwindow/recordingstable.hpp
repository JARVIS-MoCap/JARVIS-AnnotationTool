/*------------------------------------------------------------
 *  recordingstable.hpp
 *  Created: 19. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef RECORDINGSTABLE_H
#define RECORDINGSTABLE_H

#include "globals.hpp"
#include "dataset.hpp"
#include "videocutterwindow.hpp"


#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QTableWidget>
#include <QErrorMessage>

class RecordingsTable : public QWidget {
	Q_OBJECT
	public:
		explicit RecordingsTable(QString name, DatasetConfig *datasetConfig, QWidget *parent = nullptr);
		QTableWidget *recordingsTable;

		QList<RecordingItem> getItems();

	signals:

	private:
		DatasetConfig	*m_datasetConfig;
		QErrorMessage *m_errorMsg;
		QString m_name;
		int m_frameCount = 0;
		QPushButton *moveItemUpButton;
		QPushButton *moveItemDownButton;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

		QList<RecordingItem> m_recordingItems;
		int m_editingIndex;
		bool m_editingActive = false;
		QMap<int, QList<TimeLineWindow>> m_windowsMap;

		int getNumberSubfolders(QString path);
		bool isValidRecordingFolder(QString path);
		void updateTable();
		void editVideo(QString path);
		QList<QString> getVideoPaths(const QString& path);

	public slots:
		//void setNumberOfCamerasSlot(int num);

	private slots:
		void addItemSlot();
		void deleteRecordingClickedSlot();
		void editRecordingClickedSlot();
		void editingFinishedSlot(QList<TimeLineWindow> timeLineWindows, int frameCount);
		QImage createTimeLineImage(QList<TimeLineWindow> timeLineWindows);
		void nameEditedSlot(QTableWidgetItem *item);
};


#endif
