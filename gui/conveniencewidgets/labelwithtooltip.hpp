/*------------------------------------------------------------
 *  labelwithtooltip.hpp
 *  Created: 01. August 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef LABELWITHTOOLTIP_H
#define LABELWITHTOOLTIP_H

#include "globals.hpp"

#include <QLabel>
#include <QGridLayout>


class LabelWithToolTip : public QWidget {
	Q_OBJECT
	public:
		explicit LabelWithToolTip(QString name, QString toolTip = "Tip goes here", QWidget *parent = nullptr);
};

#endif
