/*------------------------------------------------------------
 *  datasetcreator.hpp
 *  Created: 23. October 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef DATASETCREATOR_H
#define DATASETCREATOR_H

#include "globals.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <QRunnable>

Q_DECLARE_METATYPE(QList<cv::Mat>)


class DatasetCreator : public QObject {
	Q_OBJECT
	public:
		explicit DatasetCreator(DatasetConfig *datasetConfig);

		signals:
			void gotAllDCTs();
			void datasetCreated();
			void datasetCreationFailed(QString errorMsg);

		public slots:
			void createDatasetSlot(QList<QString> recordings, QList<QString> entities, QList<QString> keypoints);

	private:
		DatasetConfig *m_datasetConfig;
		QList<QString> m_recordingsList;
		QList<QString> m_entitiesList;
		QList<QString> m_keypointsList;
		QMap<int,QList<cv::Mat>> m_dctMap;

		QList<QString> getCameraNames(const QString & path);
		QString getVideoFormat(const QString& recording);
		bool checkFrameCounts(const QString& recording, QList<QString> cameras);
		QList<int> extractFrames(const QString& recording, QList<QString> cameras);
		QList<QString> getAndCopyFrames(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers);
		void createSavefile(const QString& recording, QList<QString> cameraNames, const QString& dataFolder, QList<int> frameNumbers);


	private slots:
		void computedDCTsSlot(QList<cv::Mat> dctImages, int threadNumber);

};

class VideoStreamer : public QObject, public QRunnable {
	Q_OBJECT
	public:
		explicit VideoStreamer(const QString &videoPath, int threadNumber);
		void run();

	signals:
		void computedDCTs(QList<cv::Mat> dctImages, int threadNumber);

	private:
		QList<cv::Mat> dctImages;
		std::vector<cv::Mat> *m_buffer;
		int m_threadNumber;
		cv::VideoCapture *m_cap;
};

class ImageWriter : public QObject, public QRunnable {
	Q_OBJECT
	public:
	explicit ImageWriter(const QString &videoPath, const QString &destinationPath, QList<int> frameNumbers);
	void run();

	private:
		cv::VideoCapture *m_cap;
		QString m_destinationPath;
		QList<int> m_frameNumbers;
};

#endif
