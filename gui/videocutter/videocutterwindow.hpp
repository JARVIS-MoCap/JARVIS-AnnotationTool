/*------------------------------------------------------------
 *  videocutterwindow.hpp
 *  Created: 16. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef VIDEOCUTTERWINDOW_H
#define VIDEOCUTTERWINDOW_H

#include "globals.hpp"
#include "dataset.hpp"
#include "rangeslider.hpp"
#include "timeline.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QTableWidget>
#include <QDoubleSpinBox>


class VideoCutterWindow : public QWidget {
	Q_OBJECT
	public:
		explicit VideoCutterWindow(QList<TimeLineWindow> timeLineWindows = QList<TimeLineWindow>(), QWidget *parent = nullptr);
		void openVideo(const QString &path);
		virtual void closeEvent ( QCloseEvent * event );

	public slots:

	signals:
		void closingWindow();
		void editingFinished(QList<TimeLineWindow> timeLineWindows, int frameCount);

	private:
		void resizeEvent(QResizeEvent*);
		QSettings *settings;

		QMediaPlayer *player;
		QVideoWidget *videoWidget;
		int m_duration;
		int m_frameCount;
		int m_frameRate;
		RangeOverview *rangeOverview;

		ctkRangeSlider *rangeSlider;
		TimeLine *timeLine;
		QLabel *startTimeLabel;
		QLabel *endTimeLabel;

		QPushButton *playButton;
		QLabel *currentTimeLabel;
		QPushButton *slowerButton;
		QDoubleSpinBox *speedBox;

		QPushButton *fasterButton;
		QPushButton *addTimeLineElementButton;
		QWidget *saveCancelWidget;
		QPushButton *cancelElementButton;
		QPushButton *saveElementButton;
		bool m_editingWindow = false;

		QTableWidget *timeWindowTable;

		QPushButton *saveButton;
		QPushButton *loadButton;
		QPushButton *cancelButton;
		QPushButton *continueButton;

		QList<TimeLineWindow> m_timeLineWindows;
		QList<TimeLineWindow> m_initialTimeLineWindows;
		QList<TimeLineWindow> m_savedTimeLineWindows;


		int m_mainValue = 0, m_maximumValue = 0, m_minimumValue = 0;

		int m_WindowCounter = 0;

		bool addTimeLineWindow(const QString &name, int start, int end);
		void updateTimeLabels();
		bool saveSegmentation();
		bool loadSegmentation();

		private slots:
			void playerPositionChangedSlot(qint64 duration);

			void maxValueChangedSlot(int value);
			void minValueChangedSlot(int value);
			void mainValueChangedSlot(int value);

			void slowerClickedSlot();
			void fasterClickedSlot();
			void playbackSpeedChangedSlot(double value);
			void playButtonToggledSlot(bool toggle);

			void addTimeLineElementButtonClickedSlot();
			void cancelElementClickedSlot();
			void saveElementClickedSlot();
			void offsetAndZoomChangedSlot(double offset, float zoom);

			void deleteWindowClickedSlot();
			void updateTimeWindowTable();
			void timeWindowEditedSlot(QTableWidgetItem *item);

			void saveClickedSlot();
			void loadClickedSlot();
			void cancelClickedSlot();
			void continueClickedSlot();
};


#endif
