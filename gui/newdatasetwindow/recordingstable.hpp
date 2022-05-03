/*------------------------------------------------------------
 * recordingstable.hpp
 * Created: 19. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:  2022 Timo Hueser
 * License:    LGPL v2.1
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
#include <QDir>


class RecordingsTable : public QWidget {
	Q_OBJECT
	public:
		explicit RecordingsTable(QString name, DatasetConfig *datasetConfig, QWidget *parent = nullptr);
		QTableWidget *recordingsTable;

		QList<RecordingItem> getItems();

	signals:

	private:
		QErrorMessage *m_errorMsg;
		QString m_name;
		DatasetConfig	*m_datasetConfig;
		int m_frameCount = 0;
		QPushButton *moveItemUpButton;
		QPushButton *moveItemDownButton;
		QPushButton *addItemButton;
		QPushButton  *deleteItemButton;

		QList<RecordingItem> m_recordingItems;
		int m_editingIndex;
		bool m_editingActive = false;
		QDir m_currentDir;

		VideoCutterWindow *videoCutterWindow = nullptr;

		int getNumberSubfolders(QString path);
		void updateTable();
		bool editVideo(QString path);
		QList<QString> getVideoPaths(const QString& path);

	public slots:

	private slots:
		void addItemSlot();
		void deleteRecordingClickedSlot();
		void editRecordingClickedSlot();
		void editingFinishedSlot(QList<TimeLineWindow> timeLineWindows, int frameCount);
		QImage createTimeLineImage(QList<TimeLineWindow> timeLineWindows, int frameCount);
		void nameEditedSlot(QTableWidgetItem *item);
};


#endif
