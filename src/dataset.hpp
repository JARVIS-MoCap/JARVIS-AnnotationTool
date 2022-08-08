/*******************************************************************************
 * File:			  dataset.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef DATASET_H
#define DATASET_H

#include "globals.hpp"
#include "colormap.hpp"
#include "keypoint.hpp"


class Dataset : public QObject {
	Q_OBJECT

	public:
		explicit Dataset(const QString &datasetFolder,
						 const QString &datasetBaseFolder, QList<QString> cameraNames = {},
						 QList<SkeletonComponent> skeleton = {},
						 QList<QString> segmentNames = {}, bool annotateSetup = false, QList<QString> setupKeypoints = {});
		static Dataset *dataset;
		QList<ImgSet*> imgSets() {return m_imgSets;}
		const QString& datasetFolder() {return m_datasetFolder;}
		const QString& datasetBaseFolder() {return m_datasetBaseFolder;}
		const QString& cameraName(int i) {return m_cameraNames[i];}
		QList <QString> cameraNames() {return m_cameraNames;}
		QList <SkeletonComponent> skeleton() {return m_skeleton;}
		QList <QString> segmentNames() {return m_segmentNames;}
		void save(const QString& datasetFolder = "");
		int numCameras() const {return m_numCameras;}
		QList<QString> entitiesList() const {return m_entitiesList;}
		QList<QString> bodypartsList() const {return m_bodypartsList;}
		bool loadSuccessfull() const {return m_loadSuccessfull;}

	signals:
		void keypointStateChanged(KeypointState state, KeypointState previousState,
					int frameIndex);

	private:
		bool GetImageSizeEx(QString fn, int *x,int *y);

		const QString m_datasetFolder;
		const QString m_datasetBaseFolder;
		bool m_loadSuccessfull = false;
		int m_numCameras;
		int m_numEntities;
		QList <QString> m_cameraNames;
		QList <SkeletonComponent> m_skeleton;
		QList<QString> m_setupKeypointsList;
		bool m_annotateSetup;
		QList <QString> m_segmentNames;
		QList<ImgSet*> m_imgSets;
		QString m_scorer;
		QList<QString> m_keypointNameList;
		QList<QString> m_entityNameList;
		QList<QString> m_bodypartsList;
		QList<QString> m_entitiesList;
		ColorMap *m_colorMap;
};

#endif
