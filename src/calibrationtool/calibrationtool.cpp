/*******************************************************************************
 * File:			  calibrationtool.cpp
 * Created: 	  30. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/


#include "calibrationtool.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include <QThreadPool>
#include <QDir>


CalibrationTool::CalibrationTool(CalibrationConfig *calibrationConfig) :
    m_calibrationConfig(calibrationConfig) {
}


void CalibrationTool::makeCalibrationSet()  {
	m_intrinsicParameters.clear();
	m_extrinsicParameters.clear();
	QDir dir;
	dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
			m_calibrationConfig->calibrationSetName);
  delayl(100);  //Wait for GUI Thread to setup progressWindow
                //before slamming it with calibration task
  m_calibrationCanceled = false;
  m_intrinsicsReproErrors.clear();
  m_extrinsicsReproErrors.clear();
  if (!m_calibrationConfig->seperateIntrinsics) {
    m_calibrationConfig->intrinsicsPath = m_calibrationConfig->extrinsicsPath;
  }
  QThreadPool *threadPool = QThreadPool::globalInstance();
  int thread = 0;
	for (const auto& cam : m_calibrationConfig->cameraNames) {
		IntrinsicsCalibrator *intrinsicsCalibrator =
          new IntrinsicsCalibrator(m_calibrationConfig, cam, thread++);
    connect(intrinsicsCalibrator, &IntrinsicsCalibrator::intrinsicsProgress,
            this, &CalibrationTool::intrinsicsProgress);
    connect(intrinsicsCalibrator, &IntrinsicsCalibrator::finishedIntrinsics,
            this, &CalibrationTool::finishedIntrinsicsSlot);
    connect(intrinsicsCalibrator, &IntrinsicsCalibrator::calibrationError,
            this, &CalibrationTool::calibrationErrorSlot);
    connect(this, &CalibrationTool::calibrationCanceled,
            intrinsicsCalibrator,
            &IntrinsicsCalibrator::calibrationCanceledSlot);
		threadPool->start(intrinsicsCalibrator);
	}
  while (!threadPool->waitForDone(10)) {
    QCoreApplication::instance()->processEvents();
  }
	while (m_intrinsicParameters.size() != m_calibrationConfig->cameraNames.size() && !m_calibrationCanceled) {
    std::cout << "INtrinsics: " << m_intrinsicParameters.size() << " / " << m_calibrationConfig->cameraNames.size() << std::endl;
		QCoreApplication::instance()->processEvents();
	}
  std::cout << "FINISHED INTRINSICS" << std::endl;
  if (m_calibrationCanceled) return;
  threadPool->clear();
  thread = 0;
  for (const auto & pair : m_calibrationConfig->cameraPairs) {
    ExtrinsicsCalibrator *extrinsicsCalibrator =
          new ExtrinsicsCalibrator(m_calibrationConfig, m_intrinsicParameters, pair, thread++);
    connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::extrinsicsProgress,
            this, &CalibrationTool::extrinsicsProgress);
    connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::finishedExtrinsics,
            this, &CalibrationTool::finishedExtrinsicsSlot);
    connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::calibrationError,
            this, &CalibrationTool::calibrationErrorSlot);
    connect(this, &CalibrationTool::calibrationCanceled,
            extrinsicsCalibrator,
            &ExtrinsicsCalibrator::calibrationCanceledSlot);

    threadPool->start(extrinsicsCalibrator);
  }
  while (!threadPool->waitForDone(10)) {
    QCoreApplication::instance()->processEvents();
  }
	while (m_extrinsicParameters.size() != m_calibrationConfig->cameraPairs.size() && !m_calibrationCanceled) {
		std::cout << "Extrinsics: " << m_extrinsicParameters.size() << " / " << m_calibrationConfig->cameraPairs.size() << std::endl;
		QCoreApplication::instance()->processEvents();
	}
  if (!m_calibrationCanceled) {
    emit calibrationFinished();
  }
	std::cout << "ExtSize: " << m_extrinsicParameters.size() << std::endl;
  std::cout << "IntSize: " << m_intrinsicParameters.size() << std::endl;

  saveCalibration();
}


void CalibrationTool::saveCalibration() {
  for (const auto &cam : m_calibrationConfig->cameraNames) {
    cv::FileStorage fs((m_calibrationConfig->calibrationSetPath + "/" +
        m_calibrationConfig->calibrationSetName + "/" + cam + ".yaml").toStdString(),
        cv::FileStorage::WRITE);
    fs << "intrinsicMatrix" << m_intrinsicParameters[cam]["K"].t();
    fs << "distortionCoefficients" << m_intrinsicParameters[cam]["D"];
    if (m_extrinsicParameters.contains(cam)) {
      fs << "R" << m_extrinsicParameters[cam]["R"].t();
      fs << "T" << m_extrinsicParameters[cam]["T"];
    }
    else {
      fs << "R" << cv::Mat::eye(3, 3, CV_64F);
      fs << "T" << cv::Mat::zeros(3,1, CV_64F);
    }
  }
}

void CalibrationTool::finishedIntrinsicsSlot(cv::Mat K, cv::Mat D, double reproError,
      int threadNumber) {
  m_intrinsicsReproErrors[threadNumber] = reproError;
	QMap<QString, cv::Mat> intrinsics;
	intrinsics["K"] = K;
	intrinsics["D"] = D;
	m_intrinsicParameters[m_calibrationConfig->cameraNames[threadNumber]] = intrinsics;
  std::cout << "Finsihed Calib " << threadNumber << std::endl;
}


void CalibrationTool::finishedExtrinsicsSlot(cv::Mat R, cv::Mat T, double reproError,int threadNumber) {
  m_extrinsicsReproErrors[threadNumber] = reproError;
	QMap<QString, cv::Mat> extrinsics;
	extrinsics["R"] = R;
	extrinsics["T"] = T;
  int pairSize = m_calibrationConfig->cameraPairs[threadNumber].size();
	m_extrinsicParameters[m_calibrationConfig->cameraPairs[threadNumber][pairSize-1]] = extrinsics;
}


void CalibrationTool::cancelCalibrationSlot() {
  m_calibrationCanceled = true;
  emit calibrationCanceled();
}


void CalibrationTool::calibrationErrorSlot(const QString &errorMsg) {
  if(!m_calibrationCanceled) {
    m_calibrationCanceled = true;
    emit calibrationError(errorMsg);
  }
}
