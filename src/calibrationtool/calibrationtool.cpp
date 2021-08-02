/*------------------------------------------------------------
 *  calibrationtool.cpp
 *  Created: 30. July 2021
 *  Author:  Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#include "calibrationtool.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include <QThreadPool>

CalibrationTool::CalibrationTool(CalibrationConfig *calibrationConfig) :
    m_calibrationConfig(calibrationConfig) {
}

void CalibrationTool::makeCalibrationSet()  {
  std::cout << "Calibrating!!" << std::endl;

  if (m_calibrationConfig->seperateIntrinsics && m_calibrationConfig->calibrateIntrinsics) {
		QThreadPool *threadPool = QThreadPool::globalInstance();
    int thread = 0;
		for (const auto& cam : m_calibrationConfig->cameraNames) {
			IntrinsicsCalibrator *intrinsicsCalibrator = new IntrinsicsCalibrator(m_calibrationConfig, cam,thread++);
      connect(intrinsicsCalibrator, &IntrinsicsCalibrator::intrinsicsProgress, this, &CalibrationTool::intrinsicsProgress);
 			threadPool->start(intrinsicsCalibrator);
		}
    while (!threadPool->waitForDone(10)) {
      QCoreApplication::instance()->processEvents();
    }
  }
  if (!m_calibrationConfig->seperateIntrinsics || m_calibrationConfig->calibrateExtrinsics) {
    QThreadPool *threadPool = QThreadPool::globalInstance();
    int thread = 0;
    for (const auto & pair : m_calibrationConfig->cameraPairs) {
      ExtrinsicsCalibrator *extrinsicsCalibrator = new ExtrinsicsCalibrator(m_calibrationConfig, pair, thread++);
      connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::extrinsicsProgress, this, &CalibrationTool::extrinsicsProgress);
      threadPool->start(extrinsicsCalibrator);
    }
    while (!threadPool->waitForDone(10)) {
      QCoreApplication::instance()->processEvents();
    }
  }
  emit calibrationFinished();
}
