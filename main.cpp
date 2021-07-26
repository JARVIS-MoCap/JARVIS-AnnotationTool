/*------------------------------------------------------------
 *  main.cpp
 *  Created:  23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "globals.hpp"
#include "DarkStyle.hpp"
#include "mainwindow.hpp"

#include <iostream>
#include <QApplication>
#include <QSplashScreen>
#include <QSettings>
#include <QImage>
#include <QtCore/QDataStream>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(RecordingItem);
Q_DECLARE_METATYPE(TimeLineWindow);


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



int main(int argc, char **argv)
{
	// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
	#ifdef __APPLE__
			CFBundleRef mainBundle = CFBundleGetMainBundle();
			CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
			char path[PATH_MAX];
			if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
			{
					// error!
			}
			CFRelease(resourcesURL);

			chdir(path);
			std::cout << "Current Path: " << path << std::endl;
	#endif
	QCoreApplication::setOrganizationName("Test");
	QCoreApplication::setOrganizationDomain("test");
	QCoreApplication::setApplicationName("Annotation Tool");
	qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
  qRegisterMetaTypeStreamOperators<QMap<int,int> >("QMap<int,int>");
	qRegisterMetaTypeStreamOperators<QList<RecordingItem> >("QList<RecordingItem>");
	//qRegisterMetaTypeStreamOperators<QVector<QPoint> >("QVector<QPoint>");
	//LOGUTILS::initLogging(1,0);
	//qInstallMessageHandler(LOGUTILS::myMessageHandler);
	QApplication app (argc, argv);
	app.setStyle(new DarkStyle);

	QStringList themeSearchPaths = {"/home/trackingsetup/Documents/AnnotationToolbox/IconThemes", "IconThemes", "../IconThemes"};
	QIcon::setThemeSearchPaths(themeSearchPaths);
	QIcon::setThemeName("DarkIconTheme");
	MainWindow m_window;
	m_window.show();
	return app.exec();
}
