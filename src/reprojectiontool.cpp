/*------------------------------------------------------------
 *  reprojectiontool.cpp
 *  Created:  02. December 2020
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "reprojectiontool.hpp"

ReprojectionTool::ReprojectionTool(QList<QString> intrinsicsPaths, QList<QString> extrinsicsPaths, int primaryIndex) : m_primaryIndex(primaryIndex) {
	for (const auto& path : intrinsicsPaths) {
		CameraIntrinics cameraIntrinics;
		readIntrinsics(path, cameraIntrinics);
		m_cameraIntrinsicsList.append(cameraIntrinics);
	}
	for (int i = 0; i < extrinsicsPaths.size(); i++) {
		CameraExtrinsics cameraExtrinsics;
		if (i != m_primaryIndex) {
			readExtrinsincs(extrinsicsPaths[i], cameraExtrinsics);
		}
		else {
			cameraExtrinsics.rotationMatrix = cv::Mat::eye(3, 3, CV_64F);
			cameraExtrinsics.translationVector = cv::Mat::zeros(3,1, CV_64F);
			cv::vconcat(cameraExtrinsics.rotationMatrix, cameraExtrinsics.translationVector.t(),cameraExtrinsics.locationMatrix);
		}
		m_cameraExtrinsicsList.append(cameraExtrinsics);
	}
}

void ReprojectionTool::readIntrinsics(const QString& path, CameraIntrinics& cameraIntrinics) {
	cv::FileStorage fs(path.toStdString(), cv::FileStorage::READ);
	fs["intrinsicMatrix"] >> cameraIntrinics.intrinsicMatrix;
	fs["distortionCoefficients"] >> cameraIntrinics.distortionCoefficients;
}

void ReprojectionTool::readExtrinsincs(const QString& path, CameraExtrinsics& cameraExtrinsics) {
	cv::FileStorage fs(path.toStdString(), cv::FileStorage::READ);
	fs["R"] >> cameraExtrinsics.rotationMatrix;
	fs["T"] >> cameraExtrinsics.translationVector;
	fs["E"] >> cameraExtrinsics.essentialMatrix;
	fs["F"] >> cameraExtrinsics.fundamentalMatrix;
	cv::vconcat(cameraExtrinsics.rotationMatrix, cameraExtrinsics.translationVector.t(),cameraExtrinsics.locationMatrix);
}

cv::Mat ReprojectionTool::reconstructPoint3D(QList<QPointF> points, QList<int> camerasToUse) {
	QList<cv::Mat> camMats;
	for (const auto& cam : camerasToUse) {
		cv::Mat camMat = (m_cameraExtrinsicsList[cam].locationMatrix*m_cameraIntrinsicsList[cam].intrinsicMatrix).t();
		camMats.append(camMat);
	}
	cv::Mat A = cv::Mat::zeros(points.size()*2,4, CV_64F);
	for (int i = 0; i < points.size(); i++) {
		cv::Mat point = cv::Mat({points[i].x(),points[i].y()});
		A(cv::Range(2*i, 2*i+2), cv::Range::all()) = point*camMats[i](cv::Range(2,3), cv::Range::all()) - camMats[i](cv::Range(0, 2) ,cv::Range::all());
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
		cv::projectPoints(point3D, R, m_cameraExtrinsicsList[cam].translationVector,
			m_cameraIntrinsicsList[cam].intrinsicMatrix.t(), m_cameraIntrinsicsList[cam].distortionCoefficients, res);
	reprojectedPoints.append(QPointF(res.at<double>(0,0), res.at<double>(0,1)));
	}
	return reprojectedPoints;
}
