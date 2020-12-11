/*------------------------------------------------------------
 *  reprojectiontool.hpp
 *  Created: 02. December 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef REPROJECTIONTOOL_H
#define REPROJECTIONTOOL_H

#include "globals.hpp"
#include "yaml-cpp/yaml.h"

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>


class ReprojectionTool : public QObject {
	Q_OBJECT
	public:
		explicit ReprojectionTool(QList<QString> intrinsicsPaths, QList<QString> extrinsicsPaths, int primaryIndex);
		cv::Mat reconstructPoint3D(QList<QPointF> points, QList<int> camerasToUse);
		QList<QPointF> reprojectPoint(cv::Mat point3D);

	private:
		typedef struct CameraIntrinics {
			cv::Mat intrinsicMatrix;
			cv::Mat distortionCoefficients;
		} CameraIntrinics;

		typedef struct CameraExtrinsics {
			cv::Mat rotationMatrix;
			cv::Mat translationVector;
			cv::Mat locationMatrix;		//combination of rotation and Translation of secondary
			cv::Mat essentialMatrix;
			cv::Mat fundamentalMatrix;
		} CameraExtrinsics;

		void readIntrinsics(const QString& path, CameraIntrinics& cameraIntrinics);
		void readExtrinsincs(const QString& path, CameraExtrinsics& cameraExtrinsics);


		QList<CameraIntrinics> m_cameraIntrinsicsList;
		int m_primaryIndex;
		QList<CameraExtrinsics> m_cameraExtrinsicsList;



};

#endif
