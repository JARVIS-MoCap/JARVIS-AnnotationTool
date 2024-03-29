/*******************************************************************************
 * File:			  colormap.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef COLORMAP_H
#define COLORMAP_H

#include "globals.hpp"


class ColorMap : public QObject {
	Q_OBJECT

	public:
		enum ColorMapType {Jet, Fixed, Single};
		explicit ColorMap(ColorMapType type, QColor color = QColor(0,0,255));
		QColor getColor(int index, int numElements) const;

	private:
		ColorMapType m_type;
		QColor m_color;
		QVector<QColor> m_cmap;
};

#endif
