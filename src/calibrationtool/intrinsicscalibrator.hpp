/*****************************************************************
	* File:			 intrinsicscalibrator.hpp
	* Created: 	 30. July 2021
	* Author:		 Timo Hueser
	* Contact: 	 timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#ifndef INTRINSICSCALIBRATOR_H
#define INTRINSICSCALIBRATOR_H

#include "globals.hpp"

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


class IntrinsicsCalibrator : public QObject, public QRunnable {
	Q_OBJECT

	public:
		explicit IntrinsicsCalibrator(CalibrationConfig *calibrationConfig,
					const QString& cameraName, int threadNumber);
		void run();

	signals:
		void intrinsicsProgress(int counter, int frameCount, int threadNumber);
		void finishedIntrinsics(double reproError, int threadNumber);
		void calibrationError(const QString &errorMsg);

	public slots:
		void calibrationCanceledSlot();

	private:
    struct Intrinsics {
      cv::Mat K;
      cv::Mat D;
    };

		cv::Mat m_charucoPattern;
		cv::Mat m_detectedPattern;

    std::vector<cv::Point3f> m_checkerBoardPoints;
    CalibrationConfig *m_calibrationConfig;
		std::string m_parametersSavePath;
		std::string m_cameraName;
		int m_threadNumber;
		bool m_interrupt = false;
		QList<QString> m_validRecordingFormats = {"avi", "mp4", "mov", "wmv",
																							"AVI", "MP4", "WMV"};

		bool checkRotation(std::vector< cv::Point2f> &corners1, cv::Mat &img1);
		cv::Point2i getPositionOfMarkerOnBoard(std::vector< cv::Point2f>&cornersBoard, std::vector<cv::Point2f>&markerCorners);
		int matchPattern();
		bool boardToCorners(cbdetect::Board &board, cbdetect::Corner &cbCorners,
					std::vector<cv::Point2f> &corners);
		QString getFormat(const QString& path, const QString& cameraName);
		void saveCheckerboard(const cv::Mat &img, const std::vector<cv::Point2f> &corners, int counter);
};



#endif
