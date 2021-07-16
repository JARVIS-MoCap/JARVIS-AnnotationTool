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
#include "presetswindow.hpp"
#include "rangeslider.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QMediaPlayer>
#include <QVideoWidget>


class VideoCutterWindow : public QWidget {
	Q_OBJECT
	public:
		explicit VideoCutterWindow(QWidget *parent = nullptr);
		void openVideo(const QString &path);

	public slots:

	signals:

	private:
		QSettings *settings;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		QMediaPlayer *player;
		QVideoWidget *videoWidget;
		int m_duration;



		private slots:
			void savePresetsClickedSlot();
			void loadPresetsClickedSlot();
			void savePresetSlot(const QString& preset);
			void loadPresetSlot(const QString& preset);

			void maxValueChangedSlot(int value);
			void minValueChangedSlot(int value);
};


#endif
