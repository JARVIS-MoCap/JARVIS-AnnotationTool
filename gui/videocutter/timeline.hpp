/*------------------------------------------------------------
 *  timeline.hpp
 *  Created: 19. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef TIMELINE_H
#define TIMELINE_H

#include "globals.hpp"

#include <QLabel>
#include <QMouseEvent>



class TimeLine : public QWidget {
	Q_OBJECT
	public:
		explicit TimeLine(const QList<TimeLineWindow> &timeLineWindows, QWidget *parent = nullptr);
		int size();

	public slots:
		void createTimeLineImage(int size);
		void setFrameCount(int count);


	signals:
		void offsetAndZoomChanged(double offset, float zoom);


	private:
		QImage timeLineImage;
		QLabel *timeLineLabel;
		const QList<TimeLineWindow> &m_timeLineWindows;

		virtual void mousePressEvent(QMouseEvent* ev);
		virtual void mouseMoveEvent(QMouseEvent* ev);
		virtual void mouseReleaseEvent(QMouseEvent* ev);
		virtual void  wheelEvent(QWheelEvent *ev);
		bool m_dragStarted = false;
		QPointF m_startPos;
		int m_frameCount;
		double m_offset = 0;
		int m_size;
		float m_zoom = 1.0;
};

class RangeOverview : public QWidget {
	Q_OBJECT
	public:
		explicit RangeOverview(QWidget *parent = nullptr);

	public slots:
		void updateRange(int startFrame, int endFrame, int size);
		void setFrameCount(int count);

	private:
		QLabel *rangeLabel;
		QImage rangeImage;
		int m_frameCount = 1000;
};


#endif
