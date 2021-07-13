/*------------------------------------------------------------
 *  datasetcreator.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef DATASETCREATOR_H
#define DATASETCREATOR_H

#include "globals.hpp"
//#include "opencv2/opencv.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgcodecs.hpp"


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

		QList<QString> getCameraNames(const QString & path);
		QString getVideoFormat(const QString& recording);
		bool checkFrameCounts(const QString& recording, QList<QString> cameras);
		QList<int> extractFrames(const QString& recording, QList<QString> cameras);
		QList<QString> getAndCopyFrames(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers);
		void createSavefile(const QString& recording, QList<QString> cameraNames, const QString& dataFolder, QList<int> frameNumbers);


	private slots:




};

#endif
