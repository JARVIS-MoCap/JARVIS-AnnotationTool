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
			void recordingBeingProcessedChanged(QString recording);
			void currentSegmentChanged(QString segmentName);
			void dctProgress(int index, int windowSize);
			void startedClustering();
			void finishedClustering();
			void copyImagesStatus(int frameCount, int totalNumFrames);

		public slots:
			void createDatasetSlot(QList<RecordingItem> recordings, QList<QString> entities, QList<QString> keypoints);

	private:
		DatasetConfig *m_datasetConfig;
		QList<RecordingItem> m_recordingItems;
		QList<QString> m_entitiesList;
		QList<QString> m_keypointsList;
		QMap<int,QList<cv::Mat>> m_dctMap;
		QMap<int,int> m_frameNumberMap;

		QList<QString> getCameraNames(const QString & path);
		QString getVideoFormat(const QString& recording);
		bool checkFrameCounts(const QString& recording, QList<QString> cameras);
		QList<int> extractFrames(const QString &path, QList<TimeLineWindow> timeLineWindows, QList<QString> cameras);
		QList<QString> getAndCopyFrames(const QString& recording, QList<QString> cameras, const QString& dataFolder, QList<int> frameNumbers);
		void createSavefile(const QString& recording, QList<QString> cameraNames, const QString& dataFolder, QList<int> frameNumbers);
		QMap<QString, QList<TimeLineWindow>> getRecordingSubsets(QList<TimeLineWindow> timeLineWindows);


	private slots:
		void computedDCTsSlot(QList<cv::Mat> dctImages, QMap<int,int> frameNumberMap, int threadNumber);

};

class VideoStreamer : public QObject, public QRunnable {
	Q_OBJECT
	public:
		explicit VideoStreamer(const QString &videoPath, QList<TimeLineWindow> timeLineWindows, int numFramesToExtract, int threadNumber);
		void run();

	signals:
		void computedDCTs(QList<cv::Mat> dctImages, QMap<int,int> frameNumberMap, int threadNumber);
		void dctProgress(int index, int windowSize);


	private:
		QList<cv::Mat> m_dctImages;
		std::vector<cv::Mat> *m_buffer;
		int m_numFramesToExtract;
		int m_threadNumber;
  	QList<TimeLineWindow>	m_timeLineWindows;
		cv::VideoCapture *m_cap;
		QMap<int,int> m_frameNumberMap;
};

class ImageWriter : public QObject, public QRunnable {
	Q_OBJECT
	public:
		explicit ImageWriter(const QString &videoPath, const QString &destinationPath, QList<int> frameNumbers, int threadNumber);
		void run();

	signals:
		void copyImagesStatus(int frameCount, int totalNumFrames);

	private:
		cv::VideoCapture *m_cap;
		int m_threadNumber;
		QString m_destinationPath;
		QList<int> m_frameNumbers;
};

#endif
