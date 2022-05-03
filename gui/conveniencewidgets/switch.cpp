/*****************************************************************
	* File:			  switch.cpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "switch.hpp"

Switch::Switch(QWidget *parent) : QAbstractButton(parent) {
	m_anim = new QPropertyAnimation(this, "offset", this);
	setOffset(m_height / 2);
	m_y = m_height / 2;
	setBrush(QColor(100,164,32));
}


void Switch::setOffset(int o) {
	m_x = o;
	update();
}


void Switch::paintEvent(QPaintEvent *e) {
	Q_UNUSED(e);
	QPainter p(this);
	p.setPen(Qt::NoPen);
	if (isEnabled()) {
		p.setBrush(m_switch ? brush() : QColor(22, 24, 26));
		p.setOpacity(m_switch ? 0.5 : 0.38);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.drawRoundedRect(QRect(m_margin, m_margin, width() - 2 * m_margin,
														height() - 2 * m_margin), 8.0, 8.0);
		p.setBrush(m_thumb);
		p.setOpacity(1.0);
		p.drawEllipse(QRectF(offset() - (m_height / 2), m_y - (m_height / 2),
												 height(), height()));
	}
	else {
		p.setBrush(QColor(22, 24, 26));
		p.setOpacity(0.12);
		p.drawRoundedRect(QRect(m_margin, m_margin, width() - 2 * m_margin,
														height() - 2 * m_margin), 8.0, 8.0);
		p.setOpacity(1.0);
		p.setBrush(QColor("#BDBDBD"));
		p.drawEllipse(QRectF(offset() - (m_height / 2), m_y - (m_height / 2),
												 height(), height()));
	}
}


void Switch::mouseReleaseEvent(QMouseEvent *e) {
	Q_UNUSED(e);
	if (e->button() & Qt::LeftButton) {
		m_switch = m_switch ? false : true;
	  m_thumb = m_switch ? m_brush : QBrush(QColor(32, 100, 164));
		if (m_switch) {
			m_anim->setStartValue(m_height / 2);
			m_anim->setEndValue(width() - m_height);
			m_anim->setDuration(120);
			m_anim->start();
		}
		else {
			m_anim->setStartValue(offset());
			m_anim->setEndValue(m_height / 2);
			m_anim->setDuration(120);
			m_anim->start();
		}
	}
	emit toggled(m_switch);
	QAbstractButton::mouseReleaseEvent(e);
}


void Switch::enterEvent(QEvent *e) {
	setCursor(Qt::PointingHandCursor);
	QAbstractButton::enterEvent(e);
}


QSize Switch::sizeHint() const {
	return QSize(2 * (m_height + m_margin), m_height + 2 * m_margin);
}


void Switch::setToggled(bool toggle) {
	m_switch = toggle;
	m_thumb = m_switch ? m_brush : QBrush(QColor(32, 100, 164));
	if (m_switch) {
		m_anim->setStartValue(m_height / 2);
		m_anim->setEndValue(width() - m_height);
		m_anim->setDuration(120);
		m_anim->start();
	}
	else {
		m_anim->setStartValue(offset());
		m_anim->setEndValue(m_height / 2);
		m_anim->setDuration(120);
		m_anim->start();
	}
}
