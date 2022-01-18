 /*****************************************************************
	* File:			  globals.hpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

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
struct Frame {
	QString imagePath;
	QSize imageDimensions;
	int numKeypoints;
	QList<Keypoint*> keypoints;
	QMap<QString, Keypoint*> keypointMap;
};

struct ImgSet {
	int numCameras;
	QList<Frame*> frames;
};

struct SkeletonComponent {
	QString name;
	QString keypointA;
	QString keypointB;
	float length;
};
Q_DECLARE_METATYPE(SkeletonComponent)


struct DatasetConfig {
	QString datasetName = "New Dataset";
	QString datasetPath = ".";
	QString videoFormat = "";
	int numCameras= 12;
	int frameSetsRecording = 10;
	QString samplingMethod = "kmeans";
	QList<QString> validRecordingFormats = {"avi", "mp4", "mov", "wmv", "AVI", "MP4", "WMV"};
};

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
	bool debug = false;
	QString calibrationSetName;
	QString calibrationSetPath;
	bool seperateIntrinsics;
	bool calibrateExtrinsics;
	QString intrinsicsPath;
	QString extrinsicsPath;
	int maxSamplingFrameRate;
	int framesForIntrinsics;
	int framesForExtrinsics;
	QString boardType;
	int patternWidth;
	int patternHeight;
	double patternSideLength;
	QList<QString> cameraNames;
	QList<QList<QString>> cameraPairs;
	bool single_primary = false;

};

struct AnnotationCount {
	int annotated = 0;
	int reprojected = 0;
	int notAnnotated = 0;

	friend AnnotationCount operator+ (const AnnotationCount &lhs, const AnnotationCount &rhs){
			AnnotationCount res;
			res.annotated = rhs.annotated+lhs.annotated;
			res.reprojected = rhs.reprojected+lhs.reprojected;
			res.notAnnotated = rhs.notAnnotated+lhs.notAnnotated;
			return res;
	}
};

struct DatasetExportItem {
	QString name;
	QString basePath;
	QList<QPair<QString, bool>> subSets;
	AnnotationCount annotationCount;
	int frameCount = 0;
};

struct ExportConfig {
	QString trainingSetName;
	QString savePath;
	QString trainingSetType;
	QString intrinsicsPath;
	QString extrinsicsPath;
	double validationFraction;
	bool shuffleBeforeSplit;
	bool useRandomShuffleSeed;
	int shuffleSeed;
	QList<QPair<QString,bool>> entitiesList;
	QList<QPair<QString,bool>> keypointsList;
};

struct ExportKeypoint {
	QPointF point;
	KeypointState state;
};

struct ExportFrameSet {
	QString originalPath;
	QString basePath;
	int calibIndex = 0;
	QList<QString> cameras;
	QList<QString> frameNames;
	QMap<QString, QPair<QString,QList<ExportKeypoint>>> keypoints;
};



void createToolBarButton(QToolButton * button, QAction* action, QIcon icon, bool enabled = true,
												 bool checkable = false, QSize minSize = QSize(20,20));

#endif
