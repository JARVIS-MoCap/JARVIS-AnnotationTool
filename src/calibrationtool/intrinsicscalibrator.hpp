/*------------------------------------------------------------
 *  intrinsicscalibrator.hpp
 *  Created: 30. July 2021
 *  Author:  Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef INTRINSICSCALIBRATOR_H
#define INTRINSICSCALIBRATOR_H

#include "globals.hpp"

#include "boards_from_corners.h"
#include "config.h"
#include "find_corners.h"
//#include "plot_boards.h"
//#include "plot_corners.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"
#include <string>
#include <vector>

#include <QRunnable>


class IntrinsicsCalibrator : public QObject, public QRunnable {
	Q_OBJECT
	public:
		explicit IntrinsicsCalibrator(CalibrationConfig *calibrationConfig, const QString& cameraName, int threadNumber);
		void run();

	signals:
		void intrinsicsProgress(int counter, int frameCount, int threadNumber);
		void finishedIntrinsics(double reproError, int threadNumber);

	private:
    struct Intrinsics {
      cv::Mat K;
      cv::Mat D;
    };

    std::vector<cv::Point3f> m_checkerBoardPoints;
    CalibrationConfig *m_calibrationConfig;
		std::string m_parametersSavePath;
		std::string m_cameraName;
		int m_threadNumber;

		void checkRotation(std::vector< cv::Point2f> &corners1, cv::Mat &img1);
		bool boardToCorners(cbdetect::Board &board, cbdetect::Corner &cbCorners, std::vector<cv::Point2f> &corners);
};



#endif
