/*****************************************************************
	* File:			  yesnoradiowidget.hpp
	* Created: 	  12. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef YESNORADIOWIDGET_H
#define YESNORADIOWIDGET_H

#include "globals.hpp"

#include <QRadioButton>

class YesNoRadioWidget : public QWidget {
	Q_OBJECT
	public:
			explicit YesNoRadioWidget(QWidget *parent = nullptr);
			void setState(bool state);
			bool state() {return yesButton->isChecked();};

	signals:
		void stateChanged(bool state);

	private:
		QRadioButton *yesButton;
		QRadioButton *noButton;
};

#endif
