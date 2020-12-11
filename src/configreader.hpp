/*------------------------------------------------------------
 *  configreader.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include "globals.hpp"
#include "yaml-cpp/yaml.h"

class ConfigReader : public QObject {
	Q_OBJECT
	public:
		explicit ConfigReader(const QString& filename);
		QString scorer;
		QList<QString> entities;
		QList<QString> bodyparts;

	private:
		const QString m_filename;

};

#endif
