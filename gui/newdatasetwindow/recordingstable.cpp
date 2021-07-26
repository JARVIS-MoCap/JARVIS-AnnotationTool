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
	QGridLayout *layout = new QGridLayout(this);
	m_errorMsg = new QErrorMessage();

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
	connect(recordingsTable, &QTableWidget::itemChanged, this, &RecordingsTable::nameEditedSlot);

	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &RecordingsTable::addItemSlot);
	layout->addWidget(recordingsTable,0,0);
	layout->addWidget(addItemButton,1,0, Qt::AlignRight);
}

QList<RecordingItem> RecordingsTable::getItems() {
	return m_recordingItems;
}


int RecordingsTable::getNumberSubfolders(QString path) {
	int count = 0;
	for (QDirIterator it(path); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (m_datasetConfig->validRecordingFormats.contains(subpath.split('.').takeLast())) count++;
	}
	return count;
}

bool RecordingsTable::isValidRecordingFolder(QString path) {
	if (getNumberSubfolders(path) == m_datasetConfig->numCameras) {
		return true;
	}
	else {
		return false;
	}
}

void RecordingsTable::addItemSlot() {
	QString dir = QFileDialog::getExistingDirectory(this,m_name, "/media/timo/2,0 TB Volume/Recording_Sabine_17062021",
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir == "") {
		return;
	}
	if (isValidRecordingFolder(dir)) {
		RecordingItem recordingItem;
		recordingItem.name = dir.split("/").takeLast();
		recordingItem.path = dir;
		m_recordingItems.append(recordingItem);
	}
	else {
		m_errorMsg->showMessage("Selected folder is not a valid recording folder. Make sure there are exactly " +
											      QString::number(m_datasetConfig->numCameras) + " recording files in this folder.");
	}
	updateTable();
}

void RecordingsTable::updateTable() {
	recordingsTable->setIconSize(QSize(150, 20));
	recordingsTable->setRowCount(m_recordingItems.size());
	for (int i = 0; i < m_recordingItems.size(); i++) {
		QTableWidgetItem* nameItem = new QTableWidgetItem();
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
	QList<QString> videoPaths = getVideoPaths(path);
	videoCutterWindow->openVideos(videoPaths);
	connect(videoCutterWindow, &VideoCutterWindow::editingFinished, this, &RecordingsTable::editingFinishedSlot);
	videoCutterWindow->show();
}

void RecordingsTable::editingFinishedSlot(QList<TimeLineWindow> timeLineWindows, int frameCount) {
	m_frameCount = frameCount;
	m_windowsMap[m_editingIndex] = timeLineWindows;
	m_recordingItems[m_editingIndex].timeLineList = timeLineWindows;
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

QList<QString> RecordingsTable::getVideoPaths(const QString& path) {
	QList<QString> videoPaths;
	for (QDirIterator it(path); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (suffix != "." && suffix != "..") {
			suffix = suffix.split(".").takeLast();
			if (m_datasetConfig->validRecordingFormats.contains(suffix)) videoPaths.append(subpath);
		}
	}
	return videoPaths;
}

void RecordingsTable::nameEditedSlot(QTableWidgetItem *item) {
	if (item->text() != "") {
		if (item->column() == 0) {
			m_recordingItems[item->row()].name = item->text();
		}
	}
	else {
		item->setText(m_recordingItems[item->row()].name);
	}
}
