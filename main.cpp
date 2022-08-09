 /*****************************************************************
  * File:			  main.cpp
  * Created: 	  23. October 2020
  * Author:		  Timo Hueser
  * Contact: 	  timo.hueser@gmail.com
  * Copyright:  2022 Timo Hueser
  * License:    GPL v2.1
  *****************************************************************/

#include "globals.hpp"
#include "DarkStyle.hpp"
#include "mainwindow.hpp"

#include <iostream>
#include <QApplication>
#include <QSplashScreen>
#include <QSettings>
#include <QtCore/QDataStream>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

//TODO: Maybe move these to globals, seems to be the intended way
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(RecordingItem)
Q_DECLARE_METATYPE(TimeLineWindow)
Q_DECLARE_METATYPE(ExportConfig)





// // QDatastream declarations for Signal/Slot system
// QDataStream& operator<<(QDataStream& out, const RecordingItem& v) {
//     out << v.name << v.path << v.timeLineList;
//     return out;
// }

// QDataStream& operator>>(QDataStream& in, RecordingItem& v) {
//     in >> v.name;
//     in >> v.path;
// 		in >> v.timeLineList;
//     return in;
// }

// QDataStream& operator<<(QDataStream& out, const TimeLineWindow& v) {
//     out << v.name << v.start << v.end;
//     return out;
// }

// QDataStream& operator>>(QDataStream& in, TimeLineWindow& v) {
//     in >> v.name;
//     in >> v.start;
// 		in >> v.end;
//     return in;
// }

// QDataStream& operator>>(QDataStream& in, ExportConfig& v) {
//     in >> v.trainingSetName;
//     in >> v.savePath;
// 		in >> v.trainingSetType;
//     in >> v.validationFraction;
//     in >> v.shuffleBeforeSplit;
//     in >> v.useRandomShuffleSeed;
//     in >> v.shuffleSeed;
//     in >> v.entitiesList;
//     in >> v.keypointsList;
//     return in;
// }

QDataStream& operator<<(QDataStream& out, const SkeletonComponent& v) {
    out << v.name << v.keypointA << v.keypointB << v.length;
    return out;
}

QDataStream& operator>>(QDataStream& in, SkeletonComponent& v) {
    in >> v.name;
    in >> v.keypointA;
		in >> v.keypointB;
    in >> v.length;
    return in;
}

void initPresets() {
  QSettings *settings = new QSettings();
  settings->beginGroup("presetInit");
  if (!settings->value("presetInitialized_update").toBool()) {
    settings->setValue("presetInitialized_update", true);
    settings->endGroup();
    settings->beginGroup("New Dataset Window");

    QList<QString> presets = {"Hand", "RodentBody"};
    settings->setValue("Presets", QVariant::fromValue(presets));

    settings->beginGroup("Hand");
    settings->beginGroup("entitiesItemList");
    QList<QString> entItemsList = {"Hand"};
    settings->setValue("itemsList", QVariant::fromValue(entItemsList));
    settings->endGroup();
    settings->beginGroup("keypointItemList");
    QList<QString> keyItemsList = {"Pinky_T", "Pinky_D", "Pinky_M", "Pinky_P",
          "Ring_T", "Ring_D", "Ring_M", "Ring_P", "Middle_T", "Middle_D",
          "Middle_M", "Middle_P", "Index_T", "Index_D", "Index_M",
          "Index_P", "Thumb_T", "Thumb_D", "Thumb_M", "Thumb_P", "Palm",
          "Wrist_U", "Wrist_R"};
    settings->setValue("itemsList", QVariant::fromValue(keyItemsList));
    settings->endGroup();
    settings->beginGroup("skeletonItemList");
    SkeletonComponent p1 = {"Joint 1", "Pinky_T", "Pinky_D", 0};
    SkeletonComponent p2 = {"Joint 2", "Pinky_D", "Pinky_M", 0};
    SkeletonComponent p3 = {"Joint 3", "Pinky_M", "Pinky_P", 0};
    SkeletonComponent r1 = {"Joint 4", "Ring_T", "Ring_D", 0};
    SkeletonComponent r2 = {"Joint 5", "Ring_D", "Ring_M", 0};
    SkeletonComponent r3 = {"Joint 6", "Ring_M", "Ring_P", 0};
    SkeletonComponent m1 = {"Joint 7", "Middle_T", "Middle_D", 0};
    SkeletonComponent m2 = {"Joint 8", "Middle_D", "Middle_M", 0};
    SkeletonComponent m3 = {"Joint 9", "Middle_M", "Middle_P", 0};
    SkeletonComponent i1 = {"Joint 10", "Index_T", "Index_D", 0};
    SkeletonComponent i2 = {"Joint 11", "Index_D", "Index_M", 0};
    SkeletonComponent i3 = {"Joint 12", "Index_M", "Index_P", 0};
    SkeletonComponent t1 = {"Joint 13", "Thumb_T", "Thumb_D", 0};
    SkeletonComponent t2 = {"Joint 14", "Thumb_D", "Thumb_M", 0};
    SkeletonComponent t3 = {"Joint 15", "Thumb_M", "Thumb_P", 0};
    SkeletonComponent pr1 = {"Joint 16", "Pinky_P", "Ring_P", 0};
    SkeletonComponent pr2 = {"Joint 17", "Ring_P", "Middle_P", 0};
    SkeletonComponent pr3 = {"Joint 18", "Middle_P", "Index_P", 0};
    SkeletonComponent pr4 = {"Joint 19", "Index_P", "Thumb_M", 0};
    SkeletonComponent w1 = {"Joint 20", "Pinky_P", "Wrist_U", 0};
    SkeletonComponent w2 = {"Joint 21", "Wrist_U", "Wrist_R", 0};
    SkeletonComponent w3 = {"Joint 22", "Wrist_R", "Thumb_P", 0};
    QList<SkeletonComponent> skeletonItemsList = {p1,p2,p3,r1,r2,r3,m1,m2,m3,
          i1,i2,i3,t1,t2,t3,pr1,pr2,pr3,pr4,w1,w2,w3};
    settings->setValue("itemsList", QVariant::fromValue(skeletonItemsList));
    settings->endGroup();
    settings->endGroup();

    settings->beginGroup("RodentBody");
    settings->beginGroup("entitiesItemList");
    QList<QString> entItemsListRodent = {"RodentBody"};
    settings->setValue("itemsList", QVariant::fromValue(entItemsListRodent));
    settings->endGroup();
    settings->beginGroup("keypointItemList");
    QList<QString> keyItemsListRodent = {"Snout", "Right_Ear", "Left_Ear",
          "Shoulder_Center", "Right_Paw", "Right_Wrist", "Right_Elbow",
          "Right_Shoulder", "Left_Paw", "Left_Wrist", "Left_Elbow",
          "Left_Shoulder", "Hip_Center", "Right_Foot", "Right_Ankle",
          "Right_Knee", "Left_Foot", "Left_Ankle", "Left_Knee", "Tail_Tip",
          "Tail_Middle", "Tail_Root"};
    settings->setValue("itemsList", QVariant::fromValue(keyItemsListRodent));
    settings->endGroup();
    settings->beginGroup("skeletonItemList");
    SkeletonComponent j1 = {"Joint 1", "Snout", "Right_Ear", 0};
    SkeletonComponent j2 = {"Joint 2", "Snout", "Left_Ear", 0};
    SkeletonComponent j3 = {"Joint 3", "Right_Ear", "Shoulder_Center", 0};
    SkeletonComponent j4 = {"Joint 4", "Left_Ear", "Shoulder_Center", 0};
    SkeletonComponent j5 = {"Joint 5", "Right_Paw", "Right_Wrist", 0};
    SkeletonComponent j6 = {"Joint 6", "Right_Wrist", "Right_Elbow", 0};
    SkeletonComponent j7 = {"Joint 7", "Right_Elbow", "Right_Shoulder", 0};
    SkeletonComponent j8 = {"Joint 8", "Right_Shoulder", "Shoulder_Center", 0};
    SkeletonComponent j9 = {"Joint 9", "Left_Paw", "Left_Wrist", 0};
    SkeletonComponent j10 = {"Joint 10", "Left_Wrist", "Left_Elbow", 0};
    SkeletonComponent j11 = {"Joint 11", "Left_Elbow", "Left_Shoulder", 0};
    SkeletonComponent j12 = {"Joint 12", "Left_Shoulder", "Shoulder_Center", 0};
    SkeletonComponent j13 = {"Joint 13", "Shoulder_Center", "Hip_Center", 0};
    SkeletonComponent j14 = {"Joint 14", "Right_Foot", "Right_Ankle", 0};
    SkeletonComponent j15 = {"Joint 15", "Right_Ankle", "Right_Knee", 0};
    SkeletonComponent j16 = {"Joint 16", "Right_Knee", "Hip_Center", 0};
    SkeletonComponent j17 = {"Joint 17", "Left_Foot", "Left_Ankle", 0};
    SkeletonComponent j18 = {"Joint 18", "Left_Ankle", "Left_Knee", 0};
    SkeletonComponent j19 = {"Joint 19", "Left_Knee", "Hip_Center", 0};
    SkeletonComponent j20 = {"Joint 20", "Tail_Root", "Hip_Center", 0};
    QList<SkeletonComponent> skeletonItemsListRodent = {j1,j2,j3,j4,j5,j6,j7,j8,j9,
          j10,j11,j12,j13,j14,j15,j16,j17,j18,j19,j20};
    settings->setValue("itemsList", QVariant::fromValue(skeletonItemsListRodent));
    settings->endGroup();
    settings->endGroup();

    settings->endGroup();
  }
  else {
    settings->endGroup();
  }
}


int main(int argc, char **argv) {
	// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
	#ifdef __APPLE__
			CFBundleRef mainBundle = CFBundleGetMainBundle();
			CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
			char path[PATH_MAX];
			if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
			{
					std::cout << "CFURLGetFileSystemRepresentation Error" << std::endl;
			}
			CFRelease(resourcesURL);
			chdir(path);
	#endif

  qSetMessagePattern(QStringLiteral("%{time}"
        //"%{appname}"
        ": ["
        "%{if-debug}D%{endif}"
        "%{if-info}I%{endif}"
        "%{if-warning}W%{endif}"
        "%{if-critical}C%{endif}"
        "%{if-fatal}F%{endif}"
        "] "
        "%{message}"
        " ("
        "%{function} - %{file}:%{line}"
        ")"));

	QCoreApplication::setOrganizationName("JARVIS-MoCap");
	QCoreApplication::setOrganizationDomain("JARVIS-MoCap");
	QCoreApplication::setApplicationName("Annotation Tool");

	// qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
  // qRegisterMetaTypeStreamOperators<QList<QList<QString>>>("QList<QList<QString>>");
  // qRegisterMetaTypeStreamOperators<QMap<int,int> >("QMap<int,int>");
	// qRegisterMetaTypeStreamOperators<QList<RecordingItem> >("QList<RecordingItem>");
  // qRegisterMetaTypeStreamOperators<QMap<QString, double> >("QMap<QString, double>");
  // qRegisterMetaTypeStreamOperators<QMap<QString,bool> >("QMap<QString,bool>");
  // qRegisterMetaTypeStreamOperators<QList<QList<QPair<QString, bool>>>> ("QList<QList<QPair<QString, bool>>>");
  // qRegisterMetaTypeStreamOperators<QList<QPair<QString,bool>>> ("QList<QPair<QString,bool>>");
  qRegisterMetaType<ExportConfig>();
  qRegisterMetaType<SkeletonComponent>();
  qRegisterMetaType<QList<SkeletonComponent>>();
  qRegisterMetaType< cv::Mat >();
  qRegisterMetaType<QMap<QString,bool>>();
  qRegisterMetaType<QList<QList<std::pair<QString,bool>>>>();
  qRegisterMetaType<QList<QList<QString>>>();

  // qRegisterMetaTypeStreamOperators<QList<SkeletonComponent> >("QList<SkeletonComponent>");
	//qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");


	QApplication app (argc, argv);
	app.setStyle(new DarkStyle);

  initPresets();

	QStringList themeSearchPaths = {"/usr/local/share/JARVIS-AnnotationTool/icons", "/home/trackingsetup/Documents/AnnotationToolbox/IconThemes", "IconThemes", "../IconThemes"};
	QIcon::setThemeSearchPaths(themeSearchPaths);
	QIcon::setThemeName("DarkIconTheme");

	MainWindow m_window;
	m_window.show();
	return app.exec();
}
