/*****************************************************************
	* File:			  switch.hpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#ifndef SWITCH_H
#define SWITCH_H

#include <QAbstractButton>
#include <QPropertyAnimation>
#include <QPainter>
#include <QMouseEvent>


class Switch : public QAbstractButton {
    Q_OBJECT

    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

	public:
		Switch(QWidget* parent = nullptr);
		QSize sizeHint() const override;

		QBrush brush() const {return m_brush;}
		void setBrush(const QBrush &brsh) {m_brush = brsh;}
		int offset() const {return m_x;}
		void setOffset(int o);
    void setToggled(bool toggle);

  signals:
    void toggled(bool toggle);

	protected:
		void paintEvent(QPaintEvent*) override;
		void mouseReleaseEvent(QMouseEvent*) override;
		void enterEvent(QEnterEvent*) override;

	private:
		bool m_switch = false;
		qreal m_opacity = 0.0;
		int m_x, m_y;
		int m_height = 16;
		int m_margin = 3;
		QBrush m_thumb = QColor(32, 100, 164);
		QBrush m_track, m_brush;
		QPropertyAnimation *m_anim;
};

#endif
