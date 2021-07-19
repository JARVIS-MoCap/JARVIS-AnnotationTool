/*****************************************************************
 * File:			timeline.cpp
 * Created: 	18. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "timeline.hpp"

#include <QGridLayout>
#include <QApplication>

#include "opencv2/videoio/videoio.hpp"

TimeLine::TimeLine(const QList<TimeLineWindow> &timeLineWindows, QWidget *parent) : m_timeLineWindows(timeLineWindows) {
  QGridLayout *layout = new QGridLayout(this);
  m_frameCount = 1000;
  layout->setMargin(0);
  timeLineLabel = new QLabel();
  timeLineImage = QImage(m_frameCount, 1, QImage::Format_RGB888);
  layout->addWidget(timeLineLabel, 0,0);
  createTimeLineImage(500);
}

int TimeLine::size() {
  return m_size;
}

void TimeLine::createTimeLineImage(int size) {
  if (size != 0) m_size = size;
  QList<QColor> colors = {QColor(100,164,32),QColor(100,32,164), QColor(164,100,32), QColor(164,32,100), QColor(32,164,100), QColor(32,100,164)};
	for (int i = 0; i < m_frameCount; i++) {
    timeLineImage.setPixelColor(i,0,QColor(80,80,80));
    int counter = 0;
    for (const auto & window : m_timeLineWindows) {
      if (i >= (window.start*m_zoom+m_offset*m_frameCount/m_size*m_zoom) && i <= (window.end*m_zoom+m_offset*m_frameCount/m_size*m_zoom)) {
        timeLineImage.setPixelColor(i,0,colors[counter%colors.size()]);
      }
      counter++;
    }
  }
  timeLineLabel->setPixmap(QPixmap::fromImage(timeLineImage).scaled(m_size,25));
}

void TimeLine::setFrameCount(int count) {
  m_frameCount = count;
  timeLineImage = QImage(m_frameCount, 1, QImage::Format_RGB888);
}

void TimeLine::mousePressEvent(QMouseEvent* mouseEvent) {
  m_dragStarted = true;
  m_startPos = mouseEvent->pos();
}

void TimeLine::mouseMoveEvent(QMouseEvent* mouseEvent) {
  if (m_dragStarted) {
    QPointF endPos = mouseEvent->pos();
    m_offset += (endPos.rx()-m_startPos.rx())/m_zoom;
    if (m_offset > 0) m_offset = 0;
    if (m_offset < -m_size*(1.0-1.0/m_zoom)) m_offset = -m_size*(1.0-1.0/m_zoom);
    m_startPos = endPos;
    createTimeLineImage(0);
    emit offsetAndZoomChanged(m_offset, m_zoom);
  }
}

void TimeLine::mouseReleaseEvent(QMouseEvent* mouseEvent) {
  m_dragStarted = false;
}

void TimeLine::wheelEvent(QWheelEvent *event) {
  QPoint numDegrees = event->angleDelta() / 8;
  if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
    if (numDegrees.ry() > 0) m_offset += 40.0/m_zoom;
    else if (numDegrees.ry() < 0) m_offset -= 40.0/m_zoom;
    if (m_offset > 0) m_offset = 0;
    if (m_offset < -m_size*(1.0-1.0/m_zoom)) m_offset = -m_size*(1.0-1.0/m_zoom);
    createTimeLineImage(0);
    emit offsetAndZoomChanged(m_offset, m_zoom);
  }
  else {
    float side_weight = event->pos().rx();
    float old_zoom = m_zoom;
    if (numDegrees.ry() > 0) m_zoom *= 1.2;
    else if (numDegrees.ry() < 0) m_zoom /= 1.2;
    if (m_zoom < 1.0) m_zoom = 1.0;
    //if (m_offset < -m_size*(m_zoom-1.0)) m_offset = -m_size*(m_zoom-1.0);
    m_offset -= (m_size*(1./old_zoom-1./m_zoom)*(side_weight/m_size));
    if (m_offset > 0) m_offset = 0;
    if (m_offset < -m_size*(1.0-1.0/m_zoom)) m_offset = -m_size*(1.0-1.0/m_zoom);
    event->accept();
    createTimeLineImage(0);
    emit offsetAndZoomChanged(m_offset, m_zoom);
  }
}


RangeOverview::RangeOverview(QWidget *parent) {
  QGridLayout *layout = new QGridLayout(this);
  layout->setMargin(0);
  rangeLabel = new QLabel();
  layout->addWidget(rangeLabel,0,0);
}

void RangeOverview::updateRange(int startFrame, int endFrame, int size) {
  for (int i = 0; i < m_frameCount; i++) {
    if (i < startFrame || i > endFrame) {
      rangeImage.setPixelColor(i,0,QColor(80,80,80));
    }
    else {
      rangeImage.setPixelColor(i,0,QColor(100,164,32));
    }
  }
  rangeLabel->setPixmap(QPixmap::fromImage(rangeImage).scaled(size,3));
}

void RangeOverview::setFrameCount(int count) {
  m_frameCount = count;
  rangeImage = QImage(m_frameCount, 1, QImage::Format_RGB888);
}
