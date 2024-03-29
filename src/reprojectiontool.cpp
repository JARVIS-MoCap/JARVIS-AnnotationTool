/*******************************************************************************
 * File:			  reprojectiontool.cpp
 * Created: 	  02. December 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "reprojectiontool.hpp"


ReprojectionTool::ReprojectionTool(QList<QString> intrinsicsPaths,
			QList<QString> extrinsicsPaths, int primaryIndex) :
			m_primaryIndex(primaryIndex) {
	for (const auto& path : intrinsicsPaths) {
		m_cameraNames.append(path.split("/").takeLast().split(".").takeFirst());
		CameraIntrinics cameraIntrinics;
		readIntrinsics(path, cameraIntrinics);
		m_cameraIntrinsicsList.append(cameraIntrinics);
		CameraExtrinsics cameraExtrinsics;
		readExtrinsincs(path, cameraExtrinsics);
		m_cameraExtrinsicsList.append(cameraExtrinsics);
	}
}


void ReprojectionTool::readIntrinsics(const QString& path,
			CameraIntrinics& cameraIntrinics) {
	cv::FileStorage fs(path.toStdString(), cv::FileStorage::READ);
	fs["intrinsicMatrix"] >> cameraIntrinics.intrinsicMatrix;
	fs["distortionCoefficients"] >> cameraIntrinics.distortionCoefficients;
}


void ReprojectionTool::readExtrinsincs(const QString& path,
			CameraExtrinsics& cameraExtrinsics) {
	cv::FileStorage fs(path.toStdString(), cv::FileStorage::READ);
	fs["R"] >> cameraExtrinsics.rotationMatrix;
	fs["T"] >> cameraExtrinsics.translationVector;
	cv::vconcat(cameraExtrinsics.rotationMatrix,
							cameraExtrinsics.translationVector.t(),
							cameraExtrinsics.locationMatrix);
}


cv::Mat ReprojectionTool::reconstructPoint3D(QList<QPointF> points,
			QList<int> camerasToUse) {
	QList<cv::Mat> camMats;
	QList<cv::Mat> intrinsicMats;
	QList<cv::Mat> distCoefficients;

	for (const auto& cam : camerasToUse) {
		cv::Mat camMat = (m_cameraExtrinsicsList[cam].locationMatrix *
											m_cameraIntrinsicsList[cam].intrinsicMatrix).t();
		camMats.append(camMat);
		distCoefficients.append(m_cameraIntrinsicsList[cam].distortionCoefficients);
		intrinsicMats.append(m_cameraIntrinsicsList[cam].intrinsicMatrix);
	}
	cv::Mat A = cv::Mat::zeros(points.size()*2,4, CV_64F);

	for (int i = 0; i < points.size(); i++) {
		cv::Mat point = cv::Mat({points[i].x(),points[i].y()});
		cv::Mat undistPoint;
		cv::undistortPoints(point, undistPoint, intrinsicMats[i].t(),
					distCoefficients[i]);
		undistPoint.at<double>(0) = undistPoint.at<double>(0) *
					intrinsicMats[i].at<double>(0,0) +
					intrinsicMats[i].at<double>(2,0);
		undistPoint.at<double>(1) = undistPoint.at<double>(1) *
					intrinsicMats[i].at<double>(1,1) +
					intrinsicMats[i].at<double>(2,1);
		undistPoint = cv::Mat({undistPoint.at<double>(0),
	 				undistPoint.at<double>(1)});

		A(cv::Range(2*i, 2*i+2), cv::Range::all()) = undistPoint *
	 				camMats[i](cv::Range(2,3),
	 				cv::Range::all()) -
	 				camMats[i](cv::Range(0, 2),
	 				cv::Range::all());
	}
	cv::Mat w, u, vt;
	cv::SVD::compute(A, w, u, vt);
	cv::Mat X =  vt.t().col(3);
	X = X/X.at<double>(0,12);
	X = X(cv::Range(0,3), cv::Range::all());
	return X;
}


QList<QPointF> ReprojectionTool::reprojectPoint(cv::Mat point3D) {
	cv::Mat R,res;
	QList<QPointF> reprojectedPoints;
	for (int cam = 0; cam < m_cameraIntrinsicsList.size(); cam ++) {
		cv::Rodrigues(m_cameraExtrinsicsList[cam].rotationMatrix.t(), R);
		cv::Mat test;
		cv::projectPoints(point3D, R,
					m_cameraExtrinsicsList[cam].translationVector,
					m_cameraIntrinsicsList[cam].intrinsicMatrix.t(),
					m_cameraIntrinsicsList[cam].distortionCoefficients, res);
		reprojectedPoints.append(QPointF(res.at<double>(0,0), res.at<double>(0,1)));
	}
	return reprojectedPoints;
}
