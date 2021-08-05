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
  if (m_calibrationConfig->seperateIntrinsics) {
		QThreadPool *threadPool = QThreadPool::globalInstance();
    int thread = 0;
		for (const auto& cam : m_calibrationConfig->cameraNames) {
			IntrinsicsCalibrator *intrinsicsCalibrator = new IntrinsicsCalibrator(m_calibrationConfig, cam,thread++);
      connect(intrinsicsCalibrator, &IntrinsicsCalibrator::intrinsicsProgress, this, &CalibrationTool::intrinsicsProgress);
      connect(intrinsicsCalibrator, &IntrinsicsCalibrator::finishedIntrinsics, this, &CalibrationTool::finishedIntrinsicsSlot);
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
      connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::finishedExtrinsics, this, &CalibrationTool::finishedExtrinsicsSlot);
      threadPool->start(extrinsicsCalibrator);
    }
    while (!threadPool->waitForDone(10)) {
      QCoreApplication::instance()->processEvents();
    }
  }
  emit calibrationFinished();
}

void CalibrationTool::finishedIntrinsicsSlot(double reproError, int threadNumber) {
  m_intrinsicsReproErrors[threadNumber] = reproError;
}

void CalibrationTool::finishedExtrinsicsSlot(double reproError, int threadNumber) {
  m_extrinsicsReproErrors[threadNumber] = reproError;
}
