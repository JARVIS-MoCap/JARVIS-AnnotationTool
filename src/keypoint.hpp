/*------------------------------------------------------------
 *  keypoint.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef KEYPOINT_H
#define KEYPOINT_H

#include "globals.hpp"


class Keypoint : public QObject {
	Q_OBJECT
	public:
		explicit Keypoint(QString entity, QString bopdypart, QColor color = QColor(255,255,255), QPointF coordinates = QPointF(0,0));
		~Keypoint();

		void setCoordinates(QPointF coords);
		void setCoordinates(float x, float y);
		QPointF coordinates() const {return m_coordinates;}
		float rx()  {return m_coordinates.rx();}
		float ry()  {return m_coordinates.ry();}
		void setState(KeypointState state);
		const KeypointState& state() const {return m_state;}
		QString entity() const {return m_entity;}
		QString bodypart() const {return m_bodypart;}
		QString ID() const {return m_ID;}
		void setColor(QColor color) {m_color = color;}
		QColor color() const {return m_color;}
		void setShowName(bool show) {m_showName = show;}
		bool showName() const {return m_showName;}
		void setFrameIndex(int frameIndex) {m_frameIndex = frameIndex;}
		int frameIndex() const {return m_frameIndex;}


	signals:
		void stateChanged(KeypointState state, KeypointState previousState, int frameIndex);

	private:
		QPointF m_coordinates{0,0};
		KeypointState m_state = NotAnnotated;
		QString m_entity;
		QString m_bodypart;
		QString m_ID;
		QColor m_color;
		bool m_reprojected = false;
		bool m_showName = false;
		bool m_isSuppessed = false;
		int m_frameIndex = 0;


};

#endif
