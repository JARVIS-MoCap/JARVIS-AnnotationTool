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


Q_DECLARE_METATYPE(QList<int>)

int main(int argc, char **argv)
{
	QCoreApplication::setOrganizationName("Test");
	QCoreApplication::setOrganizationDomain("test");
	QCoreApplication::setApplicationName("Annotation Tool");
	//qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
	//qRegisterMetaTypeStreamOperators<QVector<QPoint> >("QVector<QPoint>");
	//LOGUTILS::initLogging(1,0);
	//qInstallMessageHandler(LOGUTILS::myMessageHandler);
	QApplication app (argc, argv);
	app.setStyle(new DarkStyle);

	QStringList themeSearchPaths = {"/home/trackingsetup/Documents/AnnotationToolbox/IconThemes"};
	QIcon::setThemeSearchPaths(themeSearchPaths);
	QIcon::setThemeName("DarkIconTheme");
	MainWindow m_window;
	m_window.show();
	return app.exec();
}
