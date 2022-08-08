/*******************************************************************************
 * File:			  reprojectiontool.hpp
 * Created: 	  02. December 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef REPROJECTIONTOOL_H
#define REPROJECTIONTOOL_H

#include "globals.hpp"

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>


class ReprojectionTool : public QObject {
	Q_OBJECT
	public:
		typedef struct CameraIntrinics {
			cv::Mat intrinsicMatrix;
			cv::Mat distortionCoefficients;
		} CameraIntrinics;

		typedef struct CameraExtrinsics {
			cv::Mat rotationMatrix;
			cv::Mat translationVector;
			cv::Mat locationMatrix;	//combination of rotation
															// and Translation of secondary
			cv::Mat essentialMatrix;
			cv::Mat fundamentalMatrix;
		} CameraExtrinsics;
		explicit ReprojectionTool(QList<QString> intrinsicsPaths,
					QList<QString> extrinsicsPaths, int primaryIndex);
		cv::Mat reconstructPoint3D(QList<QPointF> points, QList<int> camerasToUse);
		QList<QPointF> reprojectPoint(cv::Mat point3D);
		QList<QString> cameraNames() {return m_cameraNames;};
		QList<CameraExtrinsics> extrinsicsList() {return m_cameraExtrinsicsList;};
		QList<CameraIntrinics> intrinsicsList() {return m_cameraIntrinsicsList;};

	private:
		void readIntrinsics(const QString& path,
					CameraIntrinics& cameraIntrinics);

		void readExtrinsincs(const QString& path,
					CameraExtrinsics& cameraExtrinsics);

		QList<CameraIntrinics> m_cameraIntrinsicsList;
		int m_primaryIndex;
		QList<CameraExtrinsics> m_cameraExtrinsicsList;

		QList<QString> m_cameraNames;

};

#endif
