/*****************************************************************
	* File:			 extrinsicscalibrator.hpp
	* Created: 	 30. July 2021
	* Author:		 Timo Hueser
	* Contact: 	 timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef EXTRINSICSCALIBRATOR_H
#define EXTRINSICSCALIBRATOR_H

#include "globals.hpp"
#include "colormap.hpp"


#include "boards_from_corners.h"
#include "config.h"
#include "find_corners.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/aruco/charuco.hpp>
#include <string>
#include <vector>

#include <QRunnable>


class ExtrinsicsCalibrator : public QObject, public QRunnable {
	Q_OBJECT

	public:
		explicit ExtrinsicsCalibrator(CalibrationConfig *calibrationConfig, QMap<QString, QMap<QString, cv::Mat>> intrinsicParameters, QList<QString> cameraPair, int threadNumber);
		void run();

	signals:
		void extrinsicsProgress(int counter, int frameCount, int threadNumber);
		void finishedExtrinsics(cv::Mat R, cv::Mat T, double reproError, int threadNumber);
		void calibrationError(const QString &errorMsg);

	public slots:
		void calibrationCanceledSlot();

	private:
		struct Intrinsics {
			cv::Mat K;
			cv::Mat D;
		};

		struct Extrinsics {
      cv::Mat R;
      cv::Mat T;
      cv::Mat E;
      cv::Mat F;
    };

		cv::Mat m_charucoPattern;
		cv::Mat m_detectedPattern;

    std::vector<cv::Point3f> m_checkerBoardPoints;
    CalibrationConfig *m_calibrationConfig;
		QMap<QString, QMap<QString, cv::Mat>> m_intrinsicParameters;
		std::string m_parametersSavePath;
		QList<QString> m_cameraPair;
		int m_threadNumber;
		bool m_interrupt = false;
		QList<QString> m_validRecordingFormats = {"avi", "mp4", "mov", "wmv", "AVI", "MP4", "WMV"};

		bool calibrateExtrinsicsPair(QList<QString> cameraPair, Extrinsics &e, double &mean_repro_error);
		double stereoCalibrationStep(std::vector<std::vector<cv::Point3f>> &objectPoints, std::vector<std::vector<cv::Point2f>> &imagePoints1,
		      std::vector<std::vector<cv::Point2f>> &imagePoints2, Intrinsics &i1, Intrinsics &i2, Extrinsics &e, cv::Size size, double thresholdFactor);
		bool checkRotation(std::vector< cv::Point2f> &corners1, cv::Mat &img1);
		cv::Point2i getPositionOfMarkerOnBoard(std::vector< cv::Point2f>&cornersBoard, std::vector<cv::Point2f>&markerCorners);
		int matchPattern();
		bool boardToCorners(cbdetect::Board &board, cbdetect::Corner &cbCorners, std::vector<cv::Point2f> &corners);
		QString getFormat(const QString& path, const QString& cameraName);
		void saveCheckerboard(QList<QString> cameraPair, const cv::Mat &img1, const cv::Mat &img2, const std::vector<cv::Point2f> &corners1, const std::vector<cv::Point2f> &corners2, int counter);

};

#endif
