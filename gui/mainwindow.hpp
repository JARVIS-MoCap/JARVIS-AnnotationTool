/*------------------------------------------------------------
 *  mainwindow.hpp
 *  Created: 23. October 2020
 *  Author:  Timo Hüser
 *------------------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.hpp"
#include "mainbar.hpp"
#include "editorwidget.hpp"
#include "settingswindow.hpp"

#include <QMainWindow>

class MainWindow : public QMainWindow {
	Q_OBJECT
	public:
		explicit MainWindow(QMainWindow *parent = nullptr);

	public slots:
		void openSettingsWindowSlot();
		void quitClickedSlot();

	private:
		void closeEvent(QCloseEvent *event);

		SettingsWindow *settingsWindow;

		MainBar *mainBar;
		EditorWidget *editorWidget;

	private slots:
};

#endif
