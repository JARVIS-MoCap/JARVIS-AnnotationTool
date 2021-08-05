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

typedef struct DatasetConfig {
	QString datasetName = "New Dataset";
	QString datasetPath = ".";
	QString videoFormat = "";
	QString imageFormat = "";
	int numCameras= 12;
	int frameSetsRecording = 10;
	QString samplingMethod = "kmeans";
	QList<QString> validRecordingFormats = {"avi", "mp4", "mov", "wmv"};

} DatasetConfig;

struct TimeLineWindow {
	QString name;
	int start;
	int end;

	friend bool operator< (const TimeLineWindow &lhs, const TimeLineWindow &rhs){
	    if(lhs.start < rhs.start )return true;
	    else return false;
	}

	friend bool operator== (const TimeLineWindow &lhs, const TimeLineWindow &rhs){
		if (lhs.name == rhs.name && lhs.start == rhs.start && lhs.end == rhs.end) return true;
		else return false;
	}

};

struct RecordingItem {
	QString name;
	QString path;
	QList<TimeLineWindow> timeLineList;
	int frameCount = 0;
};

struct CalibrationConfig {
	QString calibrationSetName;
	QString calibrationSetPath;
	bool seperateIntrinsics;
	bool calibrateExtrinsics;
	QString intrinsicsPath;
	QString extrinsicsPath;
	int maxSamplingFrameRate;
	int framesForIntrinsics;
	int framesForExtrinsics;
	int patternWidth;
	int patternHeight;
	double patternSideLength;
	QList<QString> cameraNames;
	QList<QList<QString>> cameraPairs;
};


void createToolBarButton(QToolButton * button, QAction* action, QIcon icon, bool enabled = true,
												 bool checkable = false, QSize minSize = QSize(20,20));

#endif
