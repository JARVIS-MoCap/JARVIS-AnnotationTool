/*****************************************************************
 * File:				videocutterwindow.cpp
 * Created: 		15. July 2021
 * Author:			Timo Hueser
 * Contact: 		timo.hueser@gmail.com
 * Copyright: 	2022 Timo Hueser
 * License:   	LGPL v2.1
 *****************************************************************/

#include "videocutterwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QHeaderView>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QErrorMessage>

#include "opencv2/videoio/videoio.hpp"

VideoCutterWindow::VideoCutterWindow(QList<TimeLineWindow> timeLineWindows, QWidget *parent)
	: m_timeLineWindows(timeLineWindows), QWidget(parent, Qt::Window) {
	this->setMinimumSize(800,600);
	settings = new QSettings();
	setWindowTitle("Video Segmentation Tool");
	QGridLayout *layout = new QGridLayout(this);
	m_savedTimeLineWindows = timeLineWindows;
	m_initialTimeLineWindows = timeLineWindows;
	m_WindowCounter = m_timeLineWindows.size();

	mainSplitter = new QSplitter(this);
	mainSplitter->setOrientation(Qt::Vertical);

	QWidget *videoPlayerWidget = new QWidget();
	QGridLayout *videoplayerlayout = new QGridLayout(videoPlayerWidget);
	player = new QMediaPlayer(this);
	//player->setNotifyInterval(10);
	connect(player, &QMediaPlayer::positionChanged, this, &VideoCutterWindow::playerPositionChangedSlot);
	videoplayerlayout->setContentsMargins(0,0,0,0);
	videoplayerlayout->setSpacing(0);
	videoWidget = new QVideoWidget(this);
	videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rangeOverview = new RangeOverview();
	rangeOverview->setMinimumSize(500,3);
	videoplayerlayout->addWidget(videoWidget,0,0);
	videoplayerlayout->addWidget(rangeOverview,1,0);
	player->setVideoOutput(videoWidget);


	QWidget *controlWidget = new QWidget(mainSplitter);
	QGridLayout *controllayout = new QGridLayout(controlWidget);
  QWidget *videoControlWidget = new QWidget();
  QGridLayout *videocontrollayout = new QGridLayout(videoControlWidget);
  rangeSlider = new ctkRangeSlider(Qt::Horizontal);
  rangeSlider->setRangeVisible(false);
	connect(rangeSlider, &ctkRangeSlider::maximumValueChanged, this, &VideoCutterWindow::maxValueChangedSlot);
	connect(rangeSlider, &ctkRangeSlider::minimumValueChanged, this, &VideoCutterWindow::minValueChangedSlot);
  connect(rangeSlider, &ctkRangeSlider::mainValueChanged, this, &VideoCutterWindow::mainValueChangedSlot);

  timeLine = new TimeLine(m_timeLineWindows);
  timeLine->setMinimumSize(500,25);
	timeLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  connect(timeLine, &TimeLine::offsetAndZoomChanged, this, &VideoCutterWindow::offsetAndZoomChangedSlot);
  startTimeLabel = new QLabel("00:00");
  startTimeLabel->setMinimumSize(30,30);
  endTimeLabel = new QLabel("10:00");
  endTimeLabel->setMinimumSize(30,30);
  videocontrollayout->addWidget(rangeSlider,0,1);
  videocontrollayout->addWidget(timeLine,1,1);
  videocontrollayout->addWidget(startTimeLabel,0,0,2,1);
  videocontrollayout->addWidget(endTimeLabel,0,2,2,1);


  QWidget *buttonWidget = new QWidget();
	buttonWidget->setMaximumSize(999999,60);
  QGridLayout *buttonlayout = new QGridLayout(buttonWidget);
  playButton = new QPushButton();
  playButton->setMinimumSize(40,40);
  playButton->setMaximumSize(40,40);
  playButton->setCheckable(true);
  playButton->setIcon(QIcon::fromTheme("start"));
  connect(playButton, &QPushButton::toggled, this, &VideoCutterWindow::playButtonToggledSlot);
	currentTimeLabel = new QLabel("00:00");
	currentTimeLabel->setMinimumSize(30,30);
	QWidget *timeSpacer = new QWidget();
  timeSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	timeSpacer->setMaximumSize(30,30);
	QLabel *playBackSpeedLabel = new QLabel("Playback Speed: ");
	playBackSpeedLabel->setMinimumSize(30,30);
	slowerButton = new QPushButton();
	slowerButton->setMinimumSize(30,30);
	slowerButton->setMaximumSize(30,30);
	slowerButton->setIcon(QIcon::fromTheme("playback_tostart"));
	connect(slowerButton, &QPushButton::clicked, this, &VideoCutterWindow::slowerClickedSlot);
	speedBox = new QDoubleSpinBox();
	speedBox->setRange(0.1, 3.0);
	speedBox->setValue(player->playbackRate());
	speedBox->setMinimumSize(60,30);
	speedBox->setMaximumSize(60,30);
	connect(speedBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VideoCutterWindow::playbackSpeedChangedSlot);
	fasterButton = new QPushButton();
	fasterButton->setMinimumSize(30,30);
	fasterButton->setMaximumSize(30,30);
	fasterButton->setIcon(QIcon::fromTheme("playback_toend"));
	connect(fasterButton, &QPushButton::clicked, this, &VideoCutterWindow::fasterClickedSlot);


  QWidget *buttonSpacer = new QWidget();
  buttonSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  addTimeLineElementButton = new QPushButton("Add Segment");
  addTimeLineElementButton->setMinimumSize(40,40);
  connect(addTimeLineElementButton, &QPushButton::clicked, this, &VideoCutterWindow::addTimeLineElementButtonClickedSlot);
  saveCancelWidget = new QWidget();
  QGridLayout *savecancellayout = new QGridLayout(saveCancelWidget);
  savecancellayout->setContentsMargins(0,0,0,0);
  cancelElementButton = new QPushButton("Cancel");
	cancelElementButton->setIcon(QIcon::fromTheme("discard"));
  cancelElementButton->setMinimumSize(40,40);
  connect(cancelElementButton, &QPushButton::clicked, this, &VideoCutterWindow::cancelElementClickedSlot);
  saveElementButton = new QPushButton("Add");
	saveElementButton->setIcon(QIcon::fromTheme("plus"));
  saveElementButton->setMinimumSize(40,40);
  savecancellayout->addWidget(cancelElementButton, 0,0);
  savecancellayout->addWidget(saveElementButton, 0,1);
  connect(saveElementButton, &QPushButton::clicked, this, &VideoCutterWindow::saveElementClickedSlot);
  saveCancelWidget->hide();
  buttonlayout->addWidget(playButton, 0,0);
	buttonlayout->addWidget(currentTimeLabel, 0,1);
	buttonlayout->addWidget(timeSpacer,0,2);
	buttonlayout->addWidget(playBackSpeedLabel,0,3);
	buttonlayout->addWidget(speedBox, 0,4);
	buttonlayout->addWidget(slowerButton, 0,5);
  buttonlayout->addWidget(fasterButton, 0,6);
  buttonlayout->addWidget(buttonSpacer, 0,7);
  buttonlayout->addWidget(addTimeLineElementButton, 0,8);
  buttonlayout->addWidget(saveCancelWidget, 0,9);

  QGroupBox *timeWindowBox = new QGroupBox("Segments");
  QGridLayout *timewindowboxlayout = new QGridLayout(timeWindowBox);
  timewindowboxlayout->setContentsMargins(0,0,0,0);
  timeWindowTable = new QTableWidget(0, 5);
  timeWindowTable->setMinimumSize(200,100);
	timeWindowTable->setAlternatingRowColors(true);
	QStringList labels;
	labels << "" << "Name" << "Start" << "End" << "";
	timeWindowTable->setHorizontalHeaderLabels(labels);
	timeWindowTable->setColumnWidth(0, 20);
	timeWindowTable->horizontalHeader()-> setSectionResizeMode(1, QHeaderView::Stretch);
	timeWindowTable->verticalHeader()->hide();
	timeWindowTable->setShowGrid(false);
	timeWindowTable->setSelectionMode(QAbstractItemView::NoSelection);
  connect(timeWindowTable, &QTableWidget::itemChanged, this, &VideoCutterWindow::timeWindowEditedSlot);
  timewindowboxlayout->addWidget(timeWindowTable,0,0);

	QGroupBox *cameraSelectorBox = new QGroupBox("Camera Views");
	cameraSelectorBox->setMaximumSize(200,10000);
	QGridLayout *cameraselectorboxlayout = new QGridLayout(cameraSelectorBox);
	cameraselectorboxlayout->setContentsMargins(0,0,0,0);
	camsTable = new QTableWidget(0, 1);
	camsTable->setAlternatingRowColors(true);
	QStringList camLabels;
	camLabels << "Name";
	camsTable->setHorizontalHeaderLabels(camLabels);
	camsTable->horizontalHeader()-> setSectionResizeMode(0, QHeaderView::Stretch);
	camsTable->verticalHeader()->hide();
	camsTable->setShowGrid(false);
	camsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(camsTable, &QTableWidget::cellDoubleClicked, this, &VideoCutterWindow::changeCameraViewSlot);
	//connect(camsTable, &QTableWidget::itemPressed, this, &CamSelectorWindow::tableItemPressed);
	//connect(camsTable, &QTableWidget::itemClicked, this, &CamSelectorWindow::tableItemClicked);
	cameraselectorboxlayout->addWidget(camsTable,0,0);

  QWidget *bottomButtonWidget = new QWidget();
  QGridLayout *bottombuttonlayout = new QGridLayout(bottomButtonWidget);
	bottomButtonWidget->setMaximumSize(10000,60);
  saveButton = new QPushButton("Save");
  saveButton->setIcon(QIcon::fromTheme("upload"));
  saveButton->setMinimumSize(40,40);
  connect(saveButton, &QPushButton::clicked, this, &VideoCutterWindow::saveClickedSlot);
  loadButton = new QPushButton("Load");
  loadButton->setIcon(QIcon::fromTheme("download"));
  loadButton->setMinimumSize(40,40);
  connect(loadButton, &QPushButton::clicked, this, &VideoCutterWindow::loadClickedSlot);
  QWidget *middleSpacer = new QWidget();
  middleSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  cancelButton = new QPushButton("Cancel");
  cancelButton->setMinimumSize(40,40);
  connect(cancelButton, &QPushButton::clicked, this, &VideoCutterWindow::cancelClickedSlot);
  continueButton = new QPushButton("Continue");
  continueButton->setMinimumSize(40,40);
  connect(continueButton, &QPushButton::clicked, this, &VideoCutterWindow::continueClickedSlot);
  bottombuttonlayout->addWidget(saveButton, 0,0);
  bottombuttonlayout->addWidget(loadButton, 0,1);
  bottombuttonlayout->addWidget(middleSpacer, 0,2);
  bottombuttonlayout->addWidget(cancelButton, 0,3);
  bottombuttonlayout->addWidget(continueButton, 0,4);

	controllayout->addWidget(videoControlWidget,0,0,1,2);
  controllayout->addWidget(buttonWidget,1,0,1,2);
  controllayout->addWidget(timeWindowBox,2,0);
	controllayout->addWidget(cameraSelectorBox,2,1);

	mainSplitter->addWidget(videoPlayerWidget);
	mainSplitter->addWidget(controlWidget);

	layout->addWidget(mainSplitter,0,0);
	layout->addWidget(bottomButtonWidget,1,0);

	mainSplitter->setStretchFactor(0,2);
	this->resize(1199,800);
	QTimer::singleShot(100, this, &VideoCutterWindow::updateSize);
}

void VideoCutterWindow::updateSize() {
	this->resize(1200,800);
}

void VideoCutterWindow::openVideo(const QString &path) {
		cv::VideoCapture cap(path.toStdString());
    m_frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
    m_frameRate = cap.get(cv::CAP_PROP_FPS);
		m_duration = cap.get(cv::CAP_PROP_FRAME_COUNT)*1000/cap.get(cv::CAP_PROP_FPS);
		cap.release();
    timeLine->setFrameCount(m_frameCount);
		rangeOverview->setFrameCount(m_frameCount);
    rangeSlider->setRange(0, m_frameCount);
    rangeSlider->setMainPosition(0);
    rangeSlider->setMinimumPosition(m_frameCount*0.25);
    rangeSlider->setMaximumPosition(m_frameCount*0.75);


	//player->setMedia(QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()));
    updateTimeLabels();
    updateTimeWindowTable();

		QTimer::singleShot(10, [this]{
			timeLine->createTimeLineImage(rangeSlider->width());
			rangeOverview->setFrameCount(m_frameCount);
			rangeOverview->updateRange(rangeSlider->minimum(), rangeSlider->maximum(), videoWidget->width());
			});
}

bool VideoCutterWindow::openVideos(QList<QString> videoPaths) {
		int frameCount = 0;
		int frameRate = 0;
		QDir d = QFileInfo(videoPaths[0]).absoluteDir();
		m_recordingsPath  = d.absolutePath();
		for (const auto &path : videoPaths)
		{
			cv::VideoCapture cap(path.toStdString());
			int fc = cap.get(cv::CAP_PROP_FRAME_COUNT);
			std::cout << fc << std::endl;
			int fr = cap.get(cv::CAP_PROP_FPS);
			if (frameCount == 0) frameCount = fc;
			else if (frameCount != fc) {
				//return false;
			}
			if (frameRate == 0) frameRate = fr;
			else if (frameRate != fr) {
				//return false;
			}
			cap.release();
		}
		m_frameCount = frameCount;
		m_frameRate = frameRate;
		m_duration = m_frameCount*1000/m_frameRate;
    timeLine->setFrameCount(m_frameCount);
		rangeOverview->setFrameCount(m_frameCount);
    rangeSlider->setRange(0, m_frameCount);
    rangeSlider->setMainPosition(0);
    rangeSlider->setMinimumPosition(m_frameCount*0.25);
    rangeSlider->setMaximumPosition(m_frameCount*0.75);


	for (const auto& path : videoPaths) {
		m_playlist.append(QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()));
		m_cameraList.append(path.split('/').takeLast().split(".").takeFirst());
	}
	player->setSource(m_playlist[0]);
	player->play();
	player->pause();
    updateTimeLabels();
    updateTimeWindowTable();
		updateCameraListSlot();

		// QTimer::singleShot(10, [this]{
		// 	timeLine->createTimeLineImage(rangeSlider->width());
		// 	rangeOverview->setFrameCount(m_frameCount);
		// 	rangeOverview->updateRange(rangeSlider->minimum(), rangeSlider->maximum(), videoWidget->width());
		// });
		return true;
}

void VideoCutterWindow::updateTimeLabels() {
  int startFrame = rangeSlider->minimum();
  int endFrame = rangeSlider->maximum();
  int startMinute = startFrame/m_frameRate/60;
  int startSecond = startFrame/m_frameRate%60;
  int endMinute = endFrame/m_frameRate/60;
  int endSecond = (endFrame/m_frameRate)%60;
  QTime startTime(1, startMinute, startSecond, 0);
  startTimeLabel->setText(startTime.toString("mm:ss"));
  QTime endTime(1, endMinute, endSecond, 0);
  endTimeLabel->setText(endTime.toString("mm:ss"));
}

void VideoCutterWindow::playerPositionChangedSlot(qint64 duration) {
  if (playButton->isChecked()) {
    rangeSlider->blockSignals(true);
    rangeSlider->setMainPosition(static_cast<int>((static_cast<double>(duration)/m_duration)*m_frameCount));
    rangeSlider->blockSignals(false);
  }
	int minute = duration/1000/60;
	int second = (duration/1000)%60;
	QTime startTime(1, minute, second, 0);
	currentTimeLabel->setText(startTime.toString("mm:ss"));
}

void VideoCutterWindow::maxValueChangedSlot(int value) {
  m_maximumValue = value;
	player->setPosition(m_duration/m_frameCount*value);
}

void VideoCutterWindow::minValueChangedSlot(int value) {
  m_minimumValue = value;
	player->setPosition(m_duration/m_frameCount*value);
}

void VideoCutterWindow::mainValueChangedSlot(int value) {
  m_mainValue = value;
	player->setPosition(m_duration/m_frameCount*value);
}

void VideoCutterWindow::slowerClickedSlot() {
	if (player->playbackRate()/1.25 > 0.1) {
		player->setPlaybackRate(player->playbackRate()/1.25);
	}
	else {
		player->setPlaybackRate(0.1);
	}
	speedBox->setValue(player->playbackRate());
}

void VideoCutterWindow::fasterClickedSlot() {
	if (player->playbackRate()*1.25 < 3) {
		player->setPlaybackRate(player->playbackRate()*1.25);
	}
	else {
		player->setPlaybackRate(3.0);
	}
	speedBox->setValue(player->playbackRate());
}

void VideoCutterWindow::playbackSpeedChangedSlot(double value) {
	player->setPlaybackRate(value);
}

void VideoCutterWindow::playButtonToggledSlot(bool toggle) {
  if (toggle) {
    player->play();
    playButton->setIcon(QIcon::fromTheme("pause"));
  }
  else {
    player->pause();
    playButton->setIcon(QIcon::fromTheme("start"));
  }
}

void VideoCutterWindow::addTimeLineElementButtonClickedSlot() {
    rangeSlider->setRangeVisible(true);
    saveCancelWidget->show();
    addTimeLineElementButton->hide();
    m_editingWindow = true;
}

void VideoCutterWindow::saveElementClickedSlot() {
  if (addTimeLineWindow("", rangeSlider->minimumValue(),rangeSlider->maximumValue())) {
    rangeSlider->setRangeVisible(false);
    timeLine->createTimeLineImage(rangeSlider->width());
    saveCancelWidget->hide();
    addTimeLineElementButton->show();
    updateTimeWindowTable();
    m_editingWindow = false;
  }
}

void VideoCutterWindow::offsetAndZoomChangedSlot(double offset, float zoom) {
  rangeSlider->blockSignals(true);
  offset = offset*m_frameCount/timeLine->size();
  int minVal = -offset;
  int maxVal = minVal+m_frameCount/zoom;
  if (rangeSlider->mainVisible() && (rangeSlider->mainValue() < minVal)) {
    rangeSlider->setMainVisible(false);
  }
  else if (!rangeSlider->mainVisible() && (m_mainValue >= minVal)) {
    rangeSlider->setMainVisible(true);
  }
	if (!rangeSlider->mainOverRange() && (rangeSlider->mainValue() > maxVal)) {
		rangeSlider->setMainOverRange(true);
	}
	else if (rangeSlider->mainOverRange() && (m_mainValue <= maxVal)) {
		rangeSlider->setMainOverRange(false);
	}
  if (rangeSlider->maxInRange() && (rangeSlider->maximumValue() > maxVal)) {
    rangeSlider->setMaxInRange(false);
  }
  else if (!rangeSlider->maxInRange() && (m_maximumValue <= maxVal)) {
    rangeSlider->setMaxInRange(true);
  }
  if (rangeSlider->minInRange() && (rangeSlider->minimumValue() < minVal)) {
    rangeSlider->setMinInRange(false);
  }
  else if (!rangeSlider->minInRange() && (m_minimumValue >= minVal)) {
    rangeSlider->setMinInRange(true);
  }
  if (m_editingWindow && rangeSlider->rangeVisible() && (rangeSlider->maximumValue() < minVal || rangeSlider->minimumValue() > maxVal)) {
    rangeSlider->setRangeVisible(false);
  }
  else if (m_editingWindow && !rangeSlider->rangeVisible() && (m_maximumValue >= minVal && m_minimumValue <= maxVal)) {
    rangeSlider->setRangeVisible(true);
  }
  rangeSlider->setRange(minVal, maxVal);
  rangeSlider->blockSignals(false);
  updateTimeLabels();
	rangeOverview->updateRange(rangeSlider->minimum(), rangeSlider->maximum(), videoWidget->width());

}


void VideoCutterWindow::updateTimeWindowTable() {
  QList<QColor> colors = {QColor(100,164,32),QColor(100,32,164), QColor(164,100,32), QColor(164,32,100), QColor(32,164,100), QColor(32,100,164)};
  timeLine->createTimeLineImage(rangeSlider->width());
  timeWindowTable->setRowCount(m_timeLineWindows.size());
  for (int i = 0; i < m_timeLineWindows.size(); i++) {
    QTableWidgetItem* iconItem = new QTableWidgetItem();
    QImage img(20, 20, QImage::Format_RGB888);
    img.fill(colors[i%colors.size()]);
    iconItem->setIcon(QIcon(QPixmap::fromImage(img)));
    iconItem->setFlags(iconItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem* nameItem = new QTableWidgetItem();
    if (m_timeLineWindows[i].name == "") {
      nameItem->setText("Segment " + QString::number(m_WindowCounter));
      m_timeLineWindows[i].name = "Segment " + QString::number(m_WindowCounter++);
    }
    else {
      nameItem->setText(m_timeLineWindows[i].name);
    }
    //nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
    //nameItem->setFlags(frameItem->flags() ^ Qt::ItemIsSelectable);
    QTableWidgetItem* startItem = new QTableWidgetItem();
    int startMinute = m_timeLineWindows[i].start/m_frameRate/60;
    int startSecond = m_timeLineWindows[i].start/m_frameRate%60;
    QTime startTime(1, startMinute, startSecond, 0);
    int endMinute = m_timeLineWindows[i].end/m_frameRate/60;
    int endSecond = m_timeLineWindows[i].end/m_frameRate%60;
    QTime endTime(1, endMinute, endSecond, 0);
    startItem->setText(startTime.toString("mm:ss"));
    startItem->setFlags(startItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem* endItem = new QTableWidgetItem();
    endItem->setText(endTime.toString("mm:ss"));
    endItem->setFlags(endItem->flags() ^ Qt::ItemIsEditable);
    QPushButton *deleteButton = new QPushButton(timeWindowTable);
    //btn1->setIconSize((QSize(97,25)));
    deleteButton->setIcon(QIcon::fromTheme("discard"));
    connect(deleteButton, &QPushButton::clicked, this, &VideoCutterWindow::deleteWindowClickedSlot);
    timeWindowTable->setCellWidget(i,4,deleteButton);
    timeWindowTable->setItem(i,0,iconItem);
    timeWindowTable->setItem(i,1,nameItem);
    timeWindowTable->setItem(i,2,startItem);
    timeWindowTable->setItem(i,3,endItem);
  }
}

void VideoCutterWindow::deleteWindowClickedSlot() {
  for(int row=0; row < timeWindowTable->rowCount(); row++){
    if(sender() == timeWindowTable->cellWidget(row,4)) {
      m_timeLineWindows.removeAt(row);
    }
  }
  updateTimeWindowTable();
}

void VideoCutterWindow::timeWindowEditedSlot(QTableWidgetItem *item) {
  if (item->column() == 1) {
    m_timeLineWindows[item->row()].name = item->text();
  }
}

void VideoCutterWindow::cancelElementClickedSlot() {
    rangeSlider->setRangeVisible(false);
    saveCancelWidget->hide();
    addTimeLineElementButton->show();
    m_editingWindow = false;
}

bool VideoCutterWindow::addTimeLineWindow(const QString &name, int start, int end) {
  for (const auto & window : m_timeLineWindows) {
    int r_start = window.start;
    int r_end = window.end;
    if ((start >= r_start && start <= r_end) || (end >= r_start && end <= r_end)) {
			QErrorMessage *msg = new QErrorMessage();
			msg->showMessage("Timeline Segments are not allowed to overlap!");
      return false;
    }
  }
	if (start == end) {
		QErrorMessage *msg = new QErrorMessage();
		msg->showMessage("You are trying to save a segment that contains 0 Frames!");
		return false;
	}
  TimeLineWindow window;
  window.name = name;
  window.start = start;
  window.end = end;
  m_timeLineWindows.append(window);
	std::sort(m_timeLineWindows.begin(), m_timeLineWindows.end());
  return true;
}


void VideoCutterWindow::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);
   timeLine->createTimeLineImage(rangeSlider->width());
	 rangeOverview->updateRange(rangeSlider->minimum(), rangeSlider->maximum(), videoWidget->width());
}

void VideoCutterWindow::saveClickedSlot() {
	saveSegmentation();
}

void VideoCutterWindow::loadClickedSlot() {
	loadSegmentation();
}

void VideoCutterWindow::cancelClickedSlot() {
	if (m_initialTimeLineWindows == m_timeLineWindows) {
		close();
		return;
	}
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "", "Discard new Segmentation?\n",
                                QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		m_timeLineWindows = m_initialTimeLineWindows;
		m_savedTimeLineWindows = m_timeLineWindows;
		close();
	}
}

void VideoCutterWindow::continueClickedSlot() {
	if (!(m_savedTimeLineWindows == m_timeLineWindows)) {
		if (m_timeLineWindows.size() > 0) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "", "Save Segmentation to File?\n",
		                                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Cancel);
			if (reply == QMessageBox::Cancel) {
				return;
			}
			else if (reply == QMessageBox::Yes) {
				saveSegmentation();
			}
			else {
				m_savedTimeLineWindows = m_timeLineWindows;
			}
		}
		else {
			m_savedTimeLineWindows = m_timeLineWindows;
		}
	}
	close();
}

bool VideoCutterWindow::saveSegmentation() {
	if (m_timeLineWindows.size() == 0) {
		QErrorMessage *errorMsg = new QErrorMessage();
		errorMsg->showMessage("No Segments to save...");
		return true;
	}
	QFileDialog fd(this);
	QString fileName = fd.getSaveFileName(this, "Save Segmentation",
										  m_recordingsPath, tr("CSV Files (*.csv)"));
	fd.setAcceptMode(QFileDialog::AcceptSave);
	QFile saveFile(fileName);
	//TODO: maybe warn if file exists?
	if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&saveFile);
		out << "Name:" << "," << "Start Frame" << "," << "End Frame" << "," << "Total Number of Frames:" << "\n";
		for (const auto &window : m_timeLineWindows) {
			out << window.name << "," << window.start << "," << window.end << "," << m_frameCount << "\n";
		}
	}
	else {
		return false;
	}
	saveFile.close();
	m_savedTimeLineWindows = m_timeLineWindows;
	return true;
}

bool VideoCutterWindow::loadSegmentation() {
	QList<TimeLineWindow> timeLineWindows;
	QFileDialog fd(this);
	QString fileName = fd.getOpenFileName(this, "Load Segmentation",
										  m_recordingsPath, tr("CSV Files (*.csv)"));
	fd.setFileMode(	QFileDialog::ExistingFile);
	QFile saveFile(fileName);
	QList<QStringList> fileText;
	QErrorMessage *msg = new QErrorMessage();
	msg->resize(400,200);
	if (saveFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		int frameCount;
		QTextStream s1(&saveFile);
		while (!s1.atEnd()) {
		  QString s=s1.readLine();
		  fileText.append(s.split(","));
		}
		saveFile.close();
		for (int row = 1; row < fileText.size(); row++) {
			if (fileText[row].size() != 4) {
				msg->showMessage("Could not load read savefile. Wrong number of columns...");
				return false;
			}
			TimeLineWindow window;
			window.name = fileText[row][0];
			window.start = fileText[row][1].toInt();
			window.end = fileText[row][2].toInt();
			frameCount = fileText[row][3].toInt();
			timeLineWindows.append(window);
		}
		if (m_frameCount != frameCount) {
			msg->showMessage("Could not load segmentation savefile. Number of frames in this recording doesn't match the savefile...");
			return false;
		}
		m_timeLineWindows = timeLineWindows;
		m_savedTimeLineWindows = m_timeLineWindows;
		updateTimeLabels();
		updateTimeWindowTable();
	}
	else {
			msg->showMessage("Could not open segmentation savefile...");
			return false;
	}
}

void VideoCutterWindow::updateCameraListSlot() {
	camsTable->setRowCount(m_cameraList.size());
	int row = 0;
	for (const auto& cam : m_cameraList) {
    QTableWidgetItem* nameItem = new QTableWidgetItem();
    nameItem->setText(cam);
		nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
    camsTable->setItem(row,0,nameItem);
		row++;
	}
}

void VideoCutterWindow::changeCameraViewSlot(int row, int) {
	player->blockSignals(true);
	player = new QMediaPlayer(this);
	player->setSource(m_playlist[row]);
	player->setVideoOutput(videoWidget);
	connect(player, &QMediaPlayer::positionChanged, this, &VideoCutterWindow::playerPositionChangedSlot);
	mainValueChangedSlot(rangeSlider->mainValue());
	player->blockSignals(false);
	player->play();
	if (!playButton->isChecked()) {
		player->pause();
	}

}

void VideoCutterWindow::closeEvent (QCloseEvent * event ) {
	event->ignore();
	if (m_savedTimeLineWindows == m_timeLineWindows) {
		emit editingFinished(m_timeLineWindows, m_frameCount);
		emit closingWindow();
		event->accept();
	}
	else {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "", "Discard new Segmentation?\n",
	                                QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::Yes) {
			m_timeLineWindows = m_initialTimeLineWindows;
			m_savedTimeLineWindows = m_timeLineWindows;
			emit editingFinished(m_timeLineWindows, m_frameCount);
			emit closingWindow();
			event->accept();
		}
	}
}
