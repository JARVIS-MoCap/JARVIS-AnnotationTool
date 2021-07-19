/*****************************************************************
 * File:			recordingstable.cpp
 * Created: 	19. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "recordingstable.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QHeaderView>


RecordingsTable::RecordingsTable(QString name, DatasetConfig *datasetConfig, QWidget *parent) :
			m_name(name), m_datasetConfig(datasetConfig), QWidget(parent) {
	QGridLayout *labelselectorlayout = new QGridLayout(this);
	recordingsTable = new QTableWidget(0, 5);
	recordingsTable->setAlternatingRowColors(true);
	QStringList labels;
	labels << "Name" << "Path"  << "Segments" << "" << "";
	recordingsTable->setHorizontalHeaderLabels(labels);
	recordingsTable->horizontalHeader()-> setSectionResizeMode(1, QHeaderView::Stretch);
	recordingsTable->setColumnWidth(2, 155);
	recordingsTable->setColumnWidth(3, 30);
	recordingsTable->setColumnWidth(4, 30);
	recordingsTable->verticalHeader()->hide();
	recordingsTable->setShowGrid(false);
	recordingsTable->setSelectionMode(QAbstractItemView::NoSelection);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &RecordingsTable::addItemSlot);
	labelselectorlayout->addWidget(recordingsTable,0,0);
	labelselectorlayout->addWidget(addItemButton,1,0, Qt::AlignRight);
	this->setLayout(labelselectorlayout);
}

QList<QString> RecordingsTable::getItems() {
	QList <QString> items;
	//for (const auto& item : itemSelectorList->findItems("",Qt::MatchContains)) {
	//	items.append(item->text());
	//}
	return items;
}


int RecordingsTable::getNumberSubfolders(QString path) {
	int count = 0;
	for (QDirIterator it(path); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (m_datasetConfig->dataType == "Images" && QDir(subpath).exists() && suffix != "." && suffix != "..") count++;
		if (m_datasetConfig->dataType == "Videos" &&  subpath.split('.').takeLast() == "avi") count++;		//TODO: make this work for more formats than just avi
	}
	return count;
}

bool RecordingsTable::isValidRecordingFolder(QString path) {
	bool isValid = true;
	if (getNumberSubfolders(path) == m_datasetConfig->numCameras) {
		if (m_datasetConfig->dataType == "Images") {
			for (QDirIterator it(path); it.hasNext();) {
				QString path = it.next();
				QString suffix = path.split('/').takeLast();
				if (suffix != "." && suffix != "..") {
					if (getNumberSubfolders(path) != 0) isValid = false;
				}
			}
		}
	}
	else {
		isValid = false;
	}
	return isValid;
}

void RecordingsTable::addItemSlot() {
	QString dir = QFileDialog::getExistingDirectory(this,m_name, "/media/trackingsetup/Elements/Recordings/Recording_Colleen_03032021",
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (isValidRecordingFolder(dir)) {
		RecordingItem recordingItem;
		recordingItem.name = "Test";
		recordingItem.path = dir;
		m_recordingItems.append(recordingItem);
	}
	for (QDirIterator it(dir,  QDirIterator::Subdirectories); it.hasNext();) {
		QString path = it.next();
		QString suffix = path.split('/').takeLast();
		if (suffix != "." && suffix != "..") {
			if (isValidRecordingFolder(path)) {
				RecordingItem recordingItem;
				recordingItem.name = "TestSub";
				recordingItem.path = path;
				m_recordingItems.append(recordingItem);
			}
		}
	}
	updateTable();
}

void RecordingsTable::updateTable() {
	recordingsTable->setIconSize(QSize(150, 20));
	recordingsTable->setRowCount(m_recordingItems.size());
	for (int i = 0; i < m_recordingItems.size(); i++) {
		QTableWidgetItem* nameItem = new QTableWidgetItem();
		nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
		nameItem->setText(m_recordingItems[i].name);
		QTableWidgetItem* pathIcon = new QTableWidgetItem();
		pathIcon->setFlags(pathIcon->flags() ^ Qt::ItemIsEditable);
		pathIcon->setText(m_recordingItems[i].path);
		QPushButton *editButton = new QPushButton(recordingsTable);
    editButton->setIcon(QIcon::fromTheme("scissors"));
    connect(editButton, &QPushButton::clicked, this, &RecordingsTable::editRecordingClickedSlot);
		auto it = m_windowsMap.find(i);
		QImage timeLineImage;
		if (it != m_windowsMap.end()) {
			timeLineImage = createTimeLineImage(it.value());
		}
		else {
			timeLineImage = QImage(100, 1, QImage::Format_RGB888);
			timeLineImage.fill(QColor(80,80,80));
		}
		QTableWidgetItem* iconItem = new QTableWidgetItem();
    iconItem->setIcon(QIcon(QPixmap::fromImage(timeLineImage).scaled(150,20)));
    iconItem->setFlags(iconItem->flags() ^ Qt::ItemIsEditable);
		QPushButton *deleteButton = new QPushButton(recordingsTable);
    deleteButton->setIcon(QIcon::fromTheme("discard"));
    connect(deleteButton, &QPushButton::clicked, this, &RecordingsTable::deleteRecordingClickedSlot);
		recordingsTable->setItem(i,0,nameItem);
		recordingsTable->setItem(i,1,pathIcon);
		recordingsTable->setItem(i,2,iconItem);
		recordingsTable->setCellWidget(i,3,editButton);
		recordingsTable->setCellWidget(i,4,deleteButton);
	}
}

void RecordingsTable::deleteRecordingClickedSlot() {
	bool deletedItem = false;
	std::cout << "Rows: " << recordingsTable->rowCount() << std::endl;
	for(int row=0; row < recordingsTable->rowCount(); row++){
    if(sender() == recordingsTable->cellWidget(row,4)) {
      m_recordingItems.removeAt(row);
			m_windowsMap.remove(row);
			deletedItem = true;
    }
		else if (deletedItem == true) {
			m_windowsMap[row-1] = m_windowsMap[row];
			m_windowsMap.remove(row);
		}
  }
  updateTable();
}

void RecordingsTable::editRecordingClickedSlot() {
	for(int row=0; row < recordingsTable->rowCount(); row++){
		if(sender() == recordingsTable->cellWidget(row,3)) {
			std::cout << "Editing Video at " << row << std::endl;
			if (!m_editingActive) {
				m_editingActive = true;
				m_editingIndex = row;
				editVideo(m_recordingItems[row].path);
			}
		}
	}
}

void RecordingsTable::editVideo(QString path) {
	VideoCutterWindow * videoCutterWindow = new VideoCutterWindow(m_windowsMap[m_editingIndex]);
	videoCutterWindow->openVideo("/media/trackingsetup/Elements/Recordings/Recording_Colleen_03032021/Camera_T.avi");
	connect(videoCutterWindow, &VideoCutterWindow::editingFinished, this, &RecordingsTable::editingFinishedSlot);
	videoCutterWindow->show();
}

void RecordingsTable::editingFinishedSlot(QList<TimeLineWindow> timeLineWindows, int frameCount) {
	m_frameCount = frameCount;
	m_windowsMap[m_editingIndex] = timeLineWindows;
	std::cout << "Size: " << timeLineWindows.size() <<  ", " << frameCount <<std::endl;
	QImage timeLineImage = createTimeLineImage(timeLineWindows);
	QTableWidgetItem* iconItem = new QTableWidgetItem();
	iconItem->setIcon(QIcon(QPixmap::fromImage(timeLineImage).scaled(150,20)));
	iconItem->setFlags(iconItem->flags() ^ Qt::ItemIsEditable);
	recordingsTable->setItem(m_editingIndex,2,iconItem);
	m_editingActive = false;

}

QImage RecordingsTable::createTimeLineImage(QList<TimeLineWindow> timeLineWindows) {
	QImage timeLineImage(m_frameCount, 1, QImage::Format_RGB888);
	QList<QColor> colors = {QColor(100,164,32),QColor(100,32,164), QColor(164,100,32), QColor(164,32,100), QColor(32,164,100), QColor(32,100,164)};
	for (int i = 0; i < m_frameCount; i++) {
    int counter = 0;
		timeLineImage.setPixelColor(i,0,QColor(80,80,80));
    for (const auto & window : timeLineWindows) {
      if (i >= window.start && i <= window.end) {
        timeLineImage.setPixelColor(i,0,colors[counter%colors.size()]);
      }
      counter++;
    }
	}
	return timeLineImage;
}
