/*******************************************************************************
 * File:			  datasetcreator.hpp
 * Created: 	  10. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/

#ifndef DATASETCREATOR_H
#define DATASETCREATOR_H

#include "globals.hpp"
#include "imagewriter.hpp"
#include "videostreamer.hpp"

#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "yaml-cpp/yaml.h"

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
		void recordingBeingProcessedChanged(QString recording,
					QList<QString> cameras);
		void currentSegmentChanged(QString segmentName);
		void dctProgress(int index, int windowSizeint, int threadNumber);
		void startedClustering();
		void finishedClustering();
		void copyImagesStatus(int frameCount, int totalNumFrames, int threadNumber);
		void creationCanceled();

	public slots:
		void createDatasetSlot(QList<RecordingItem> recordings,
					QList<QString> entities, QList<QString> keypoints,
					QList<SkeletonComponent> skeleton);
		void cancelCreationSlot();

	private:
		DatasetConfig *m_datasetConfig;
		QList<RecordingItem> m_recordingItems;
		QList<QString> m_entitiesList;
		QList<QString> m_keypointsList;
		QList<SkeletonComponent> m_skeleton;
		QMap<int,QList<cv::Mat>> m_dctMap;
		QMap<int,int> m_frameNumberMap;
		bool m_creationCanceled = false;

		void createDatasetConfigFile(const QString& path);
		QList<QString> getCameraNames(const QString & path);
		QString getVideoFormat(const QString& recording);
		bool checkFrameCounts(const QString& recording, QList<QString> cameras);
		QList<int> extractFrames(const QString &path,
					QList<TimeLineWindow> timeLineWindows, QList<QString> cameras);
		QList<QString> getAndCopyFrames(const QString& recording,
					QList<QString> cameras, const QString& dataFolder,
					QList<int> frameNumbers);
		void createSavefile(const QString& recording, QList<QString> cameraNames,
					const QString& dataFolder, QList<int> frameNumbers);
		QMap<QString, QList<TimeLineWindow>> getRecordingSubsets(
					QList<TimeLineWindow> timeLineWindows);

	private slots:
		void computedDCTsSlot(QList<cv::Mat> dctImages,
					QMap<int,int> frameNumberMap, int threadNumber);

};

#endif
