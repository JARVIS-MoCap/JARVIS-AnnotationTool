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
  delayl(100);  //Wait for GUI Thread to setup progressWindow before potentially slamming it with calibration task
  m_calibrationCanceled = false;
  m_intrinsicsReproErrors.clear();
  m_extrinsicsReproErrors.clear();
  if (m_calibrationConfig->seperateIntrinsics) {
    QThreadPool *threadPool = QThreadPool::globalInstance();
    int thread = 0;
		for (const auto& cam : m_calibrationConfig->cameraNames) {
			IntrinsicsCalibrator *intrinsicsCalibrator = new IntrinsicsCalibrator(m_calibrationConfig, cam, thread++);
      connect(intrinsicsCalibrator, &IntrinsicsCalibrator::intrinsicsProgress, this, &CalibrationTool::intrinsicsProgress);
      connect(intrinsicsCalibrator, &IntrinsicsCalibrator::finishedIntrinsics, this, &CalibrationTool::finishedIntrinsicsSlot);
      connect(this, &CalibrationTool::calibrationCanceled, intrinsicsCalibrator, &IntrinsicsCalibrator::calibrationCanceledSlot);
 			threadPool->start(intrinsicsCalibrator);
		}
    while (!threadPool->waitForDone(10)) {
      QCoreApplication::instance()->processEvents();
    }
  }
  if (m_calibrationCanceled) return;
  if (!m_calibrationConfig->seperateIntrinsics || m_calibrationConfig->calibrateExtrinsics) {
    QThreadPool *threadPool = QThreadPool::globalInstance();
    int thread = 0;
    for (const auto & pair : m_calibrationConfig->cameraPairs) {
      ExtrinsicsCalibrator *extrinsicsCalibrator = new ExtrinsicsCalibrator(m_calibrationConfig, pair, thread++);
      connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::extrinsicsProgress, this, &CalibrationTool::extrinsicsProgress);
      connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::finishedExtrinsics, this, &CalibrationTool::finishedExtrinsicsSlot);
      connect(extrinsicsCalibrator, &ExtrinsicsCalibrator::calibrationError, this, &CalibrationTool::calibrationErrorSlot);
      connect(this, &CalibrationTool::calibrationCanceled, extrinsicsCalibrator, &ExtrinsicsCalibrator::calibrationCanceledSlot);

      threadPool->start(extrinsicsCalibrator);
    }
    while (!threadPool->waitForDone(10)) {
      QCoreApplication::instance()->processEvents();
    }
  }
  if (!m_calibrationCanceled) {
    emit calibrationFinished();
  }
}

void CalibrationTool::finishedIntrinsicsSlot(double reproError, int threadNumber) {
  m_intrinsicsReproErrors[threadNumber] = reproError;
}

void CalibrationTool::finishedExtrinsicsSlot(double reproError, QMap<QString, double> intrinsicsErrorMap,int threadNumber) {
  m_extrinsicsReproErrors[threadNumber] = reproError;
  if (intrinsicsErrorMap.size() == 2) {
    for (const auto & key : intrinsicsErrorMap.keys()) {
      std::cout << key.toStdString() << std::endl;
      m_intrinsicsReproErrors[m_calibrationConfig->cameraNames.indexOf(key)] = intrinsicsErrorMap[key];
    }
    //intrinsicsErrorMap[m_calibrationConfig->cameraNames.indexOf(intrinsicsErrorMap)]
  }
}

void CalibrationTool::cancelCalibrationSlot() {
  m_calibrationCanceled = true;
  emit calibrationCanceled();
}

void CalibrationTool::calibrationErrorSlot(const QString &errorMsg) {
  std::cout << "Here" << std::endl;
  if(!m_calibrationCanceled) {
    m_calibrationCanceled = true;
    emit calibrationError(errorMsg);
  }
}
