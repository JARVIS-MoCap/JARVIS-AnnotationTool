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

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

Q_DECLARE_METATYPE(QList<int>)


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
