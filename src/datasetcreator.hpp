/*------------------------------------------------------------
 *  datasetcreator.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef DATASETCREATOR_H
#define DATASETCREATOR_H

#include "globals.hpp"


class DatasetCreator : public QObject {
	Q_OBJECT
	public:
		explicit DatasetCreator(DatasetConfig *datasetConfig);

		signals:

		public slots:
			void createDatasetSlot(QList<QString> recordings, QList<QString> entities, QList<QString> keypoints);

	private:
		DatasetConfig *m_datasetConfig;
		QList<QString> m_recordingsList;
		QList<QString> m_entitiesList;
		QList<QString> m_keypointsList;

		void createSavefile(const QString& dataFolder, QList<QString> frames);
		QList<QString> getCameraNames(QString path);


	private slots:




};

#endif
