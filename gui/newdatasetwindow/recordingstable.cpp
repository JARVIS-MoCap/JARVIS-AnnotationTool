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
	layout->setMargin(3);
	m_errorMsg = new QErrorMessage();

	m_currentDir = QDir(QDir::homePath());

	recordingsTable = new QTableWidget(0, 5);
	recordingsTable->setAlternatingRowColors(true);
	QStringList labels;
	labels << "Name" << "Path"  << "Segments" << "" << "";
	recordingsTable->setHorizontalHeaderLabels(labels);
	recordingsTable->horizontalHeader()-> setSectionResizeMode(0, QHeaderView::Stretch);
	recordingsTable->horizontalHeader()-> setSectionResizeMode(1, QHeaderView::ResizeToContents);

	recordingsTable->setColumnWidth(2, 155);
	recordingsTable->setColumnWidth(3, 30);
	recordingsTable->setColumnWidth(4, 30);
	recordingsTable->verticalHeader()->hide();
	recordingsTable->setShowGrid(false);
	recordingsTable->setSelectionMode(QAbstractItemView::NoSelection);
	connect(recordingsTable, &QTableWidget::itemChanged, this, &RecordingsTable::nameEditedSlot);

	addItemButton = new QPushButton("Add Recording");
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


void RecordingsTable::addItemSlot() {

	QString dir = QFileDialog::getExistingDirectory(this,m_name, m_currentDir.path(),
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir == "") {
		return;
	}
	int numValidRecordings = getNumberSubfolders(dir);
	if (numValidRecordings != 0) {
		RecordingItem recordingItem;
		m_datasetConfig->numCameras = numValidRecordings;
		recordingItem.name = dir.split("/").takeLast();
		recordingItem.path = dir;
		m_recordingItems.append(recordingItem);
		m_currentDir = QDir(dir);
		m_currentDir.cdUp();
	}
	else {
		m_errorMsg->showMessage("Selected folder is not a valid recording folder.");
	}
	updateTable();
}

void RecordingsTable::updateTable() {
	recordingsTable->setIconSize(QSize(150, 20));
	recordingsTable->setRowCount(m_recordingItems.size());
	std::cout << m_recordingItems.size() << std::endl;
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
			timeLineImage = createTimeLineImage(it.value(), m_recordingItems[i].frameCount);
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
		if (m_editingActive && i == m_editingIndex) deleteButton->setEnabled(false);
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
			if (row < m_editingIndex) m_editingIndex--;
			std::cout << "Row " << row << std::endl;
      m_recordingItems.removeAt(row);
			m_windowsMap.remove(row);
			deletedItem = true;
    }
		else if (deletedItem == true) {
			auto it = m_windowsMap.find(row);
			if (it != m_windowsMap.end()) {
					m_windowsMap[row-1] = m_windowsMap[row];
					m_windowsMap.remove(row);
			}
		}
  }
  updateTable();
}

void RecordingsTable::editRecordingClickedSlot() {
	for(int row=0; row < recordingsTable->rowCount(); row++){
		if(sender() == recordingsTable->cellWidget(row,3)) {
			if (!m_editingActive) {
				m_editingActive = true;
				m_editingIndex = row;
				recordingsTable->cellWidget(row,4)->setEnabled(false);
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
	m_recordingItems[m_editingIndex].frameCount = frameCount;
	QImage timeLineImage = createTimeLineImage(timeLineWindows, frameCount);
	QTableWidgetItem* iconItem = new QTableWidgetItem();
	iconItem->setIcon(QIcon(QPixmap::fromImage(timeLineImage).scaled(150,20)));
	iconItem->setFlags(iconItem->flags() ^ Qt::ItemIsEditable);
	recordingsTable->setItem(m_editingIndex,2,iconItem);
	m_editingActive = false;
	recordingsTable->cellWidget(m_editingIndex,4)->setEnabled(true);

}

QImage RecordingsTable::createTimeLineImage(QList<TimeLineWindow> timeLineWindows, int frameCount) {
	QImage timeLineImage(frameCount, 1, QImage::Format_RGB888);
	QList<QColor> colors = {QColor(100,164,32),QColor(100,32,164), QColor(164,100,32), QColor(164,32,100), QColor(32,164,100), QColor(32,100,164)};
	for (int i = 0; i < frameCount; i++) {
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
	if (item->column() == 0) {
		if (item->text() != "") {
			m_recordingItems[item->row()].name = item->text();
		}
		else {
			item->setText(m_recordingItems[item->row()].name);
		}
	}
}
