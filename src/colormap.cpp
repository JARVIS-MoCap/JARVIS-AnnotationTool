/*------------------------------------------------------------
 *  colormap.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "colormap.hpp"

ColorMap::ColorMap(ColorMapType type, QColor color)
			: m_type(type), m_color(color) {
	if(type == Jet) {
		int r,g,b;
	  for (int i = 0; i < 255; i++) {
			float n=4.0*i/256.0;
			r = 255.0*std::min(std::max(std::min(n-1.5,-n+4.5),0.0),1.0);
			g = 255.0*std::min(std::max(std::min(n-0.5,-n+3.5),0.0),1.0);
			b = 255.0*std::min(std::max(std::min(n+0.5,-n+2.5),0.0),1.0);
			m_cmap.append(QColor(r,g,b));
		}
	}
	else if(type == Fixed) {
		for (int i = 0; i < 255; i++) {
			m_cmap.append(m_color);
		}
	}
	else if(type == Single) {
		int r,g,b;
		int diff_r = 255 - color.red();
		int diff_g = 255 - color.green();
		int diff_b = 255 - color.blue();

		for (int i = 0; i < 255; i++) {
			r = color.red() + diff_r * (i/255.0);
			g = color.green() + diff_g * (i/255.0);
			b = color.blue() + diff_b * (i/255.0);
			m_cmap.append(QColor(r,g,b));
		}
	}

}

QColor ColorMap::getColor(int index, int numElements) {
	int cmapIndex = index*(256/numElements);
	return m_cmap[cmapIndex];
}
