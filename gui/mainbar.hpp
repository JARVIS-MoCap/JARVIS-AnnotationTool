/*******************************************************************************
 * File:			  mainbar.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef MAINBAR_H
#define MAINBAR_H

#include "globals.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QToolBar>


class MainBar : public QToolBar {
	Q_OBJECT

	public:
		explicit MainBar(QWidget *parent = nullptr);

	signals:
		void openSettingsWindow();
		void openHelpWindow();
		void exitToMainPage();

	private:
		QToolButton *settingsButton;
		QAction *settingsAction;
		QToolButton *helpButton;
		QAction *helpAction;
		QToolButton *exitButton;
		QAction *exitAction;
};

#endif
