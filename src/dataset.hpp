/*------------------------------------------------------------
 *  dataset.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef DATASET_H
#define DATASET_H

#include "globals.hpp"
#include "colormap.hpp"
#include "keypoint.hpp"


class Dataset : public QObject {
	Q_OBJECT
	public:
		explicit Dataset(const QString& datasetFolder);
		static Dataset *dataset;
		QList<ImgSet*> imgSets() {return m_imgSets;}
		const QString& cameraName(int i) {return m_cameraNames[i];}
		QList <QString> cameraNames() {return m_cameraNames;}
		void save(const QString& datasetFolder = "");
		int numCameras() const {return m_numCameras;}
		QList<QString> entitiesList() const {return m_entitiesList;}
		QList<QString> bodypartsList() const {return m_bodypartsList;}

		signals:
			void keypointStateChanged(KeypointState state, KeypointState previousState, int frameIndex);

	private:
		bool GetImageSizeEx(QString fn, int *x,int *y);

		const QString m_datasetFolder;
		int m_numCameras;
		int m_numEntities;
		QList <QString> m_cameraNames;
		QList<ImgSet*> m_imgSets;
		QString m_scorer;
		QList<QString> m_keypointNameList;
		QList<QString> m_entityNameList;
		QList<QString> m_bodypartsList;
		QList<QString> m_entitiesList;
		ColorMap *m_colorMap;


};

#endif
