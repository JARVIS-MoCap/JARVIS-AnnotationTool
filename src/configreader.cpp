/*------------------------------------------------------------
 *  configreader.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "configreader.hpp"
#include <string>


ConfigReader::ConfigReader(const QString& filename)
			: m_filename(filename) {
	YAML::Node config = YAML::LoadFile(m_filename.toStdString());

	if (config["scorer"]) {
	  scorer = QString::fromStdString(config["scorer"].as<std::string>());
	}
	else {
		scorer = "default";
	}

	if (config["entities"]) {
		for (const auto& entity : config["entities"]) {
			entities.append(QString::fromStdString(entity.as<std::string>()));
		}
	}
	else {
		std::cout << "Could not find entities List" << std::endl;
	}

	if (config["bodyparts"]) {
		for (const auto& bp : config["bodyparts"]) {
			bodyparts.append(QString::fromStdString(bp.as<std::string>()));
		}
	}
	else {
		std::cout << "Could not find bodyparts List" << std::endl;
	}

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals

	//<-> Relayed Signals
}
