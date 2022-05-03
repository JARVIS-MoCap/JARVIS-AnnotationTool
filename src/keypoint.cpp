/*******************************************************************************
 * File:			  keypoint.cpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "keypoint.hpp"


Keypoint::Keypoint(QString entity,QString bodypart, QColor color,
			QPointF coordinates) : m_entity(entity), m_bodypart(bodypart),
			m_color(color), m_coordinates(coordinates) {
	m_ID = m_entity + "/" + m_bodypart;
}

Keypoint::~Keypoint() {
}


void Keypoint::setCoordinates(QPointF point) {
	m_coordinates = point;
}


void Keypoint::setState(KeypointState state) {
	if (m_state != state) {
		if (m_state == Reprojected) {
			m_color.setAlpha(100);
		}
		else if (m_state == Annotated) {
			m_color.setAlpha(255);
		}
		emit stateChanged(state, m_state, m_frameIndex);
		m_state = state;
	}
}
