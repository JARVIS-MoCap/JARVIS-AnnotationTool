/*------------------------------------------------------------
 *  extrinsicscalibrator.hpp
 *  Created: 30. July 2021
 *  Author:  Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef EXTRINSICSCALIBRATOR_H
#define EXTRINSICSCALIBRATOR_H

#include "globals.hpp"

#include "boards_from_corners.h"
#include "config.h"
#include "find_corners.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"
#include <string>
#include <vector>

#include <QRunnable>


class ExtrinsicsCalibrator : public QObject, public QRunnable {
	Q_OBJECT
	public:
		explicit ExtrinsicsCalibrator(CalibrationConfig *calibrationConfig, QList<QString> cameraPair, int threadNumber);
		void run();

	signals:
		void extrinsicsProgress(int counter, int frameCount, int threadNumber);
		void finishedExtrinsics(double reproError, QMap<QString, double> intrinsicsErrorMap, int threadNumber);

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

    std::vector<cv::Point3f> m_checkerBoardPoints;
    CalibrationConfig *m_calibrationConfig;
		std::string m_parametersSavePath;
		QList<QString> m_cameraPair;
		int m_threadNumber;
		bool m_interrupt = false;
		QList<QString> m_validRecordingFormats = {"avi", "mp4", "mov", "wmv"};

		void calibrateExtrinsicsPair(QList<QString> cameraPair, Extrinsics &e);
		double stereoCalibrationStep(std::vector<std::vector<cv::Point3f>> &objectPoints, std::vector<std::vector<cv::Point2f>> &imagePoints1,
		      std::vector<std::vector<cv::Point2f>> &imagePoints2, Intrinsics &i1, Intrinsics &i2, Extrinsics &e, cv::Size size, double thresholdFactor);
		double calibrateIntrinsicsStep(std::string cameraName, std::vector<std::vector<cv::Point3f>> objectPoints, std::vector<std::vector<cv::Point2f>> imagePoints,
		        cv::Size size, Intrinsics &intrinsics);
		void checkRotation(std::vector< cv::Point2f> &corners1, cv::Mat &img1);
		bool boardToCorners(cbdetect::Board &board, cbdetect::Corner &cbCorners, std::vector<cv::Point2f> &corners);
		bool tryLoadIntrinsics(QList<QString> cameraPair, Intrinsics &i1, Intrinsics &i2);
		QString getFormat(const QString& path, const QString& cameraName);

};



#endif
