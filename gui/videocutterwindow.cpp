/*****************************************************************
 * File:			videocutterwindow.cpp
 * Created: 	15. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "videocutterwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>

#include "opencv2/videoio/videoio.hpp"



#include <QProxyStyle>

class SliderProxy : public QProxyStyle
{
public:
    int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
    {
        switch(metric) {
        case PM_SliderThickness  : return 30;
        case PM_SliderLength     : return 30;
        default                         : return (QProxyStyle::pixelMetric(metric,option,widget));
        }
    }
};


VideoCutterWindow::VideoCutterWindow(QWidget *parent) : QWidget(parent, Qt::Window) {
	this->resize(800,800);
	this->setMinimumSize(600,600);
	settings = new QSettings();
	setWindowTitle("Video Cutter");
	QGridLayout *layout = new QGridLayout(this);

	loadPresetsWindow = new PresetsWindow(&presets, "load", "VideoCutter/");
	savePresetsWindow = new PresetsWindow(&presets, "save", "VideoCutter/");
	connect(loadPresetsWindow, SIGNAL(loadPreset(QString)), this, SLOT(loadPresetSlot(QString)));
	connect(savePresetsWindow, SIGNAL(savePreset(QString)), this, SLOT(savePresetSlot(QString)));

	player = new QMediaPlayer(this);
	videoWidget = new QVideoWidget(this);

	ctkRangeSlider * slider = new ctkRangeSlider(Qt::Horizontal);
	SliderProxy *aSliderProxy = new SliderProxy();
	slider->setStyle(aSliderProxy);
	connect(slider, &ctkRangeSlider::maximumValueChanged, this, &VideoCutterWindow::maxValueChangedSlot);

	connect(slider, &ctkRangeSlider::minimumValueChanged, this, &VideoCutterWindow::minValueChangedSlot);

	layout->addWidget(videoWidget,0,0);
	layout->addWidget(slider,1,0);

	openVideo("/home/trackingsetup/Videos/Ralph_Test_13072021/Videos/Camera_T.avi");
}

void VideoCutterWindow::openVideo(const QString &path) {
		cv::VideoCapture cap(path.toStdString());
		m_duration = cap.get(cv::CAP_PROP_FRAME_COUNT)*1000/cap.get(cv::CAP_PROP_FPS);
		cap.release();
		player->setMedia(QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()));
		player->setVideoOutput(videoWidget);
		player->play();
		player->pause();
}

void VideoCutterWindow::maxValueChangedSlot(int value) {
	player->setPosition(m_duration/100*value);
}


void VideoCutterWindow::minValueChangedSlot(int value) {
	player->setPosition(m_duration/100*value);
}


void VideoCutterWindow::savePresetsClickedSlot() {
	savePresetsWindow->updateListSlot();
	savePresetsWindow->show();
}


void VideoCutterWindow::loadPresetsClickedSlot() {
	loadPresetsWindow->updateListSlot();
	loadPresetsWindow->show();
}


void VideoCutterWindow::savePresetSlot(const QString& preset) {
	/*settings->beginGroup(preset);
	settings->beginGroup("entitiesItemList");
	QList<QString> entItemsList;
	for (const auto& item : entitiesItemList->itemSelectorList->findItems("",Qt::MatchContains)) {
		entItemsList.append(item->text());
	}
	settings->setValue("itemsList", QVariant::fromValue(entItemsList));
	settings->endGroup();
	settings->beginGroup("keypointItemList");
	QList<QString> keyItemsList;
	for (const auto& item : keypointsItemList->itemSelectorList->findItems("",Qt::MatchContains)) {
		keyItemsList.append(item->text());
	}
	settings->setValue("itemsList", QVariant::fromValue(keyItemsList));
	settings->endGroup();
	settings->endGroup();*/
}


void VideoCutterWindow::loadPresetSlot(const QString& preset) {
	/*entitiesItemList->itemSelectorList->clear();
	keypointsItemList->itemSelectorList->clear();
	settings->beginGroup(preset);
	settings->beginGroup("entitiesItemList");
	QList<QString>entIitemsList = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : entIitemsList) {
		entitiesItemList->itemSelectorList->addItem(item);
	}
	settings->endGroup();
	settings->beginGroup("keypointItemList");
	QList<QString> keyItemsList = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : keyItemsList) {
		keypointsItemList->itemSelectorList->addItem(item);
	}
	settings->endGroup();
	settings->endGroup();*/
}
