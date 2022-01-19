/*******************************************************************************
 * File:			  calibrationtool.hpp
 * Created: 	  30. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/

#ifndef CALIBRATIONTOOL_H
#define CALIBRATIONTOOL_H

#include "globals.hpp"
#include "intrinsicscalibrator.hpp"
#include "extrinsicscalibrator.hpp"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"
#include <string>
#include <vector>
#include <iostream>


class CalibrationTool : public QObject {
	Q_OBJECT

  public:
    explicit CalibrationTool(CalibrationConfig *calibrationConfig);
		QMap<int, double> getIntrinsicsReproErrors() {
			return m_intrinsicsReproErrors;
		};
		QMap<int, double> getExtrinsicsReproErrors() {
			return m_extrinsicsReproErrors;
		};

	signals:
    void intrinsicsProgress(int counter, int frameCount, int threadNumber);
    void extrinsicsProgress(int counter, int frameCount, int threadNumber);
    void calibrationFinished();
		void calibrationCanceled();
		void calibrationError(const QString & errorMsg);

  public slots:
    void makeCalibrationSet();
		void cancelCalibrationSlot();

  private:
		void saveCalibration();

    CalibrationConfig *m_calibrationConfig;
		QMap<int, double> m_intrinsicsReproErrors;
		QMap<int, double> m_extrinsicsReproErrors;
		QMap<QString, QMap<QString, cv::Mat>> m_intrinsicParameters;
		QMap<QString, QMap<QString, cv::Mat>> m_extrinsicParameters;
		bool m_calibrationCanceled = false;

	private slots:
		void finishedIntrinsicsSlot(cv::Mat K, cv::Mat D, double reproError, int threadNumber);
		void finishedExtrinsicsSlot(cv::Mat R, cv::Mat T, double reproError, int threadNumber);
		void calibrationErrorSlot(const QString &errorMsg);
};


#endif
