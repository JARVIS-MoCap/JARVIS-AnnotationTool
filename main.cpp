 /*****************************************************************
  * File:			  main.cpp
  * Created: 	  23. October 2020
  * Author:		  Timo Hueser
  * Contact: 	  timo.hueser@gmail.com
  * Copyright:  2021 Timo Hueser
  * License:    GPL v3.0
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

//TODO: Maybe move these to glabels, seems to be the intended way
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(RecordingItem)
Q_DECLARE_METATYPE(TimeLineWindow)
Q_DECLARE_METATYPE(ExportConfig)



// QDatastream declarations for Signal/Slot system
QDataStream& operator<<(QDataStream& out, const RecordingItem& v) {
    out << v.name << v.path << v.timeLineList;
    return out;
}

QDataStream& operator>>(QDataStream& in, RecordingItem& v) {
    in >> v.name;
    in >> v.path;
		in >> v.timeLineList;
    return in;
}

QDataStream& operator<<(QDataStream& out, const TimeLineWindow& v) {
    out << v.name << v.start << v.end;
    return out;
}

QDataStream& operator>>(QDataStream& in, TimeLineWindow& v) {
    in >> v.name;
    in >> v.start;
		in >> v.end;
    return in;
}

QDataStream& operator>>(QDataStream& in, ExportConfig& v) {
    in >> v.trainingSetName;
    in >> v.savePath;
		in >> v.trainingSetType;
    in >> v.intrinsicsPath;
    in >> v.extrinsicsPath;
    in >> v.validationFraction;
    in >> v.shuffleBeforeSplit;
    in >> v.useRandomShuffleSeed;
    in >> v.shuffleSeed;
    in >> v.entitiesList;
    in >> v.keypointsList;
    return in;
}

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


int main(int argc, char **argv)
{
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

	QCoreApplication::setOrganizationName("Test");
	QCoreApplication::setOrganizationDomain("test");
	QCoreApplication::setApplicationName("Annotation Tool");

	qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
  qRegisterMetaTypeStreamOperators<QList<QList<QString>>>("QList<QList<QString>>");
  qRegisterMetaTypeStreamOperators<QMap<int,int> >("QMap<int,int>");
	qRegisterMetaTypeStreamOperators<QList<RecordingItem> >("QList<RecordingItem>");
  qRegisterMetaTypeStreamOperators<QMap<QString, double> >("QMap<QString, double>");
  qRegisterMetaTypeStreamOperators<QMap<QString,bool> >("QMap<QString,bool>");
  qRegisterMetaTypeStreamOperators<QList<QList<QPair<QString, bool>>>> ("QList<QList<QPair<QString, bool>>>");
  qRegisterMetaTypeStreamOperators<QList<QPair<QString,bool>>> ("QList<QPair<QString,bool>>");
  qRegisterMetaType<ExportConfig>("ExportConfig");
  //qRegisterMetaType<SkeletonComponent>("SkeletonComponent");
  qRegisterMetaTypeStreamOperators<QList<SkeletonComponent> >("QList<SkeletonComponent>");

	QApplication app (argc, argv);
	app.setStyle(new DarkStyle);

	QStringList themeSearchPaths = {"/usr/local/share/AnnotationTool/icons", "/home/trackingsetup/Documents/AnnotationToolbox/IconThemes", "IconThemes", "../IconThemes"};
	QIcon::setThemeSearchPaths(themeSearchPaths);
	QIcon::setThemeName("DarkIconTheme");

	MainWindow m_window;
	m_window.show();
	return app.exec();
}
