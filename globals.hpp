/*------------------------------------------------------------
 *  globals.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <stdint.h>
#include <QCoreApplication>
#include <QTime>
#include <QToolButton>
#include <QAction>

class Keypoint;

inline void delayl(int ms) {
	//delay function that doesn't interrupt the QEventLoop
	QTime dieTime = QTime::currentTime().addMSecs(ms);
	while (QTime::currentTime() < dieTime) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

enum KeypointState {NotAnnotated, Annotated, Reprojected, Suppressed};
enum KeypointShape {Circle, Rectangle, Triangle};
//----- Structs Definitions ----//
typedef struct Frame {
	QString imagePath;
	QSize imageDimensions;
	int numKeypoints;
	QList<Keypoint*> keypoints;
	QMap<QString, Keypoint*> keypointMap;
}Frame;

typedef struct ImgSet {
	int numCameras;
	QList<Frame*> frames;
} ImgSet;



void createToolBarButton(QToolButton * button, QAction* action, QIcon icon, bool enabled = true,
												 bool checkable = false, QSize minSize = QSize(20,20));

#endif
