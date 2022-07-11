/*******************************************************************************
 * File:		controldock.hpp
 * Created: 	  07. July 2022
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef CONTROLDOCK_H
#define CONTROLDOCK_H

#include "globals.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QDockWidget>
#include <QPushButton>
#include <QGridLayout>



class ControlDock : public QDockWidget {
	Q_OBJECT

	public:
		explicit ControlDock(QWidget *parent = nullptr);
        void resetCameraButtons();
        void updateCameraButtons(const QList<QString> cameraNames);

    public slots:

	signals:
        void loadClicked();
        void saveClicked();
        void cameraViewChanged(int index);

	private:
        QWidget *mainWidget;
        QWidget *cameraButtonWidget;
		QList<QPushButton*> cameraButtonArray;
		QGridLayout *camerabuttonlayout;

    private slots:
        void cameraViewChangedSlot();

};

#endif
