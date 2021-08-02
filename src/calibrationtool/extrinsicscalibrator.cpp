/*------------------------------------------------------------
 *  extrinsicscalibrator.cpp
 *  Created: 30. July 2021
 *  Author:  Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#include "extrinsicscalibrator.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include <QThreadPool>
#include <QDir>

ExtrinsicsCalibrator::ExtrinsicsCalibrator(CalibrationConfig *calibrationConfig, QList<QString> cameraPair, int threadNumber) :
  m_calibrationConfig(calibrationConfig), m_cameraPair(cameraPair), m_threadNumber(threadNumber){
  QDir dir;
  dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" + m_calibrationConfig->calibrationSetName + "/Intrinsics");
  dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" + m_calibrationConfig->calibrationSetName + "/Extrinsics");
  m_parametersSavePath = (m_calibrationConfig->calibrationSetPath + "/" + m_calibrationConfig->calibrationSetName).toStdString();
}


void ExtrinsicsCalibrator::run() {
  int numCameras = m_cameraPair.size();
  Extrinsics extrinsics1, extrinsics2;

  if (numCameras == 2) {
    std::cout << "Calibrating Pair Camera_" << m_cameraPair[0].toStdString() << " and Camera_" << m_cameraPair[1].toStdString() << std::endl;
    calibrateExtrinsicsPair(m_cameraPair, extrinsics1);
    cv::FileStorage fse(m_parametersSavePath + "/Extrinsics/Extrinsics_" + m_cameraPair[0].toStdString() + "_" + m_cameraPair[1].toStdString() + ".yaml", cv::FileStorage::WRITE);
    fse << "R" << extrinsics1.R.t();
    fse << "T" << extrinsics1.T;
    fse << "E" << extrinsics1.E;
    fse << "F" << extrinsics1.F;
  }
  else if (numCameras == 3) {
    std::cout << "Calibrating Pair Camera_" << m_cameraPair[0].toStdString() << " and Camera_" << m_cameraPair[2].toStdString() << std::endl;
    QList<QString> cameraPair1 = {m_cameraPair[0], m_cameraPair[1]};
    QList<QString> cameraPair2 = {m_cameraPair[1], m_cameraPair[2]};
    calibrateExtrinsicsPair(cameraPair1, extrinsics1);
    calibrateExtrinsicsPair(cameraPair2, extrinsics2);
    cv::Mat T1_t = extrinsics1.R.t() * extrinsics1.T;
    cv::Mat T2_t = extrinsics2.R.t() * extrinsics2.T;
    cv::Mat R = extrinsics2.R*extrinsics1.R;
    cv::Mat T = R*(T1_t + extrinsics1.R.t()*T2_t);
    cv::FileStorage fse(m_parametersSavePath + "/Extrinsics/Extrinsics_" + m_cameraPair[0].toStdString() + "_" + m_cameraPair[2].toStdString() + ".yaml", cv::FileStorage::WRITE);
    fse << "R" << R.t();
    fse << "T" << T;
    fse << "E" << extrinsics1.E;
    fse << "F" << extrinsics1.F;
  }
}


void ExtrinsicsCalibrator::calibrateExtrinsicsPair(QList<QString> cameraPair, Extrinsics &e) {
  std::cout << "Detecting Checkerboards for Camera_" << cameraPair[0].toStdString() << " and Camera_" << cameraPair[1].toStdString() << "..." << std::endl;
  std::vector<cv::Point3f> checkerBoardPoints;
  for (int i = 0; i < m_calibrationConfig->patternHeight; i++)
    for (int j = 0; j < m_calibrationConfig->patternWidth; j++)
      checkerBoardPoints.push_back(cv::Point3f((float)j * m_calibrationConfig->patternSideLength, (float)i * m_calibrationConfig->patternSideLength, 0));
  cv::VideoCapture cap1((m_calibrationConfig->extrinsicsPath + "/" + cameraPair[0] + "-" + cameraPair[1] + "/" + cameraPair[0] + ".avi").toStdString());
  cv::VideoCapture cap2((m_calibrationConfig->extrinsicsPath + "/" + cameraPair[0] + "-" + cameraPair[1] + "/" + cameraPair[1] + ".avi").toStdString());
  int frameCount = cap1.get(cv::CAP_PROP_FRAME_COUNT);

  std::vector<std::vector<cv::Point3f>> objectPointsAll, objectPoints;
  std::vector<std::vector<cv::Point2f>> imagePointsAll1, imagePointsAll2, imagePoints1, imagePoints2;
  std::vector<cv::Point2f> corners1, corners2;
	cv::Size size;
  cbdetect::Corner cbCorners1, cbCorners2;
  std::vector<cbdetect::Board> boards1, boards2;
  cbdetect::Params params;
  params.corner_type = cbdetect::SaddlePoint;
  params.show_processing = false;
  params.show_debug_image = false;

  bool read_success = true;
  int counter = 0;
  while (read_success) {
    cv::Mat img1,img2;
    bool read_success1 = cap1.read(img1);
    bool read_success2 = cap2.read(img2);
    read_success = read_success1 && read_success2;
    if (read_success) {
      int frameIndex = cap1.get(cv::CAP_PROP_POS_FRAMES);
      cap1.set(cv::CAP_PROP_POS_FRAMES, frameIndex+30);
      cap2.set(cv::CAP_PROP_POS_FRAMES, frameIndex+30);
      corners1.clear();
      corners2.clear();
      size = img1.size();

      cbdetect::find_corners(img1, cbCorners1, params);
      cbdetect::boards_from_corners(img1, cbCorners1, boards1, params);
      cbdetect::find_corners(img2, cbCorners2, params);
      cbdetect::boards_from_corners(img2, cbCorners2, boards2, params);

      bool patternFound1 = (cbCorners1.p.size() >= m_calibrationConfig->patternHeight*m_calibrationConfig->patternWidth);
      bool patternFound2 = (cbCorners2.p.size() >= m_calibrationConfig->patternHeight*m_calibrationConfig->patternWidth);
      if (patternFound1 && patternFound2) {
        patternFound1 = boardToCorners(boards1[0], cbCorners1, corners1);
        patternFound2 = boardToCorners(boards2[0], cbCorners2, corners2);
        if (patternFound1 && patternFound2) {
          checkRotation(corners1, img1);
          checkRotation(corners2, img2);
          imagePointsAll1.push_back(corners1);
          imagePointsAll2.push_back(corners2);
          objectPointsAll.push_back(checkerBoardPoints);
        }
      }
      emit extrinsicsProgress(counter*40, frameCount, m_threadNumber);
      counter++;
    }
  }
  double keep_ratio = imagePointsAll1.size() / (double)std::min( m_calibrationConfig->framesForExtrinsics, (int)imagePointsAll1.size());
  for (double k = 0; k < imagePointsAll1.size(); k += keep_ratio) {
    imagePoints1.push_back(imagePointsAll1[(int)k]);
    imagePoints2.push_back(imagePointsAll2[(int)k]);
    objectPoints.push_back(objectPointsAll[(int)k]);
  }
  std::cout << "Calibrating Camera_" << cameraPair[0].toStdString() << " and Camera_" << cameraPair[1].toStdString() << " using "<< imagePoints1.size() << " Images..." << std::endl;

  Intrinsics i1,i2;
  bool intrinsicsFound = tryLoadIntrinsics(cameraPair, i1,i2);
  if (intrinsicsFound) {
    std::cout << "Found precomputed Intrinsics for both Cameras..." << std::endl;
  }
  else {
    calibrateIntrinsicsStep(cameraPair[0].toStdString(), objectPoints, imagePoints1, size, i1);
    calibrateIntrinsicsStep(cameraPair[1].toStdString(), objectPoints, imagePoints2, size, i2);
  }

  double mean_repro_error = stereoCalibrationStep(objectPoints, imagePoints1, imagePoints2, i1,i2,e, size, 1.2);
  std::cout << "Mean Reprojection Error after Stage 1: " << mean_repro_error << std::endl;
  std::cout << "Number Images for Stage 2: " <<imagePoints1.size() << std::endl;

  mean_repro_error = stereoCalibrationStep(objectPoints, imagePoints1, imagePoints2, i1,i2,e, size, 1.4);
  std::cout << "Mean Reprojection Error after Stage 2: " << mean_repro_error << std::endl;
  std::cout << "Final Number Images: " <<imagePoints1.size() << std::endl;

  cv::Mat errs;
  mean_repro_error = cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2, i1.K, i1.D, i2.K, i2.D, size, e.R, e.T, e.E, e.F,errs,
        cv::CALIB_FIX_INTRINSIC, cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 120, 1e-7));
  std::cout << "Final Mean Reprojection Error: " << mean_repro_error << std::endl;
}

double ExtrinsicsCalibrator::stereoCalibrationStep(std::vector<std::vector<cv::Point3f>> &objectPoints, std::vector<std::vector<cv::Point2f>> &imagePoints1,
      std::vector<std::vector<cv::Point2f>> &imagePoints2, Intrinsics &i1, Intrinsics &i2, Extrinsics &e, cv::Size size, double thresholdFactor) {
  cv::Mat errs;
  double mean_repro_error  = cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2, i1.K, i1.D, i2.K, i2.D, size, e.R, e.T, e.E, e.F,errs,
        cv::CALIB_FIX_INTRINSIC, cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 80, 1e-6));

  std::vector< std::vector< cv::Point3f > > objectPoints_2;
  std::vector< std::vector< cv::Point2f > > imagePoints1_2, imagePoints2_2;
  for (int i = 0; i < errs.size[0]; i++) {
    if (std::max(errs.at<double>(i,0), errs.at<double>(i,1)) < thresholdFactor*mean_repro_error) {
      imagePoints1_2.push_back(imagePoints1[i]);
      imagePoints2_2.push_back(imagePoints2[i]);
      objectPoints_2.push_back(objectPoints[i]);
    }
  }
  imagePoints1 = imagePoints1_2;
  imagePoints2 = imagePoints2_2;
  objectPoints = objectPoints_2;
  return mean_repro_error;
}

void ExtrinsicsCalibrator::calibrateIntrinsicsStep(std::string cameraName, std::vector<std::vector<cv::Point3f>> objectPoints, std::vector<std::vector<cv::Point2f>> imagePoints,
        cv::Size size, Intrinsics &intrinsics) {
  std::cout << "Calibrating Camera_" << cameraName << " using "<< imagePoints.size() << " Images..." << std::endl;
  std::vector< cv::Mat > rvecs, tvecs;
  double repro_error = calibrateCamera(objectPoints, imagePoints, size, intrinsics.K, intrinsics.D, rvecs, tvecs,
    cv::CALIB_FIX_K3 | cv::CALIB_ZERO_TANGENT_DIST, cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 80, 1e-6));
  std::cout << "Camera_" << cameraName <<  " calibrated with Reprojection Error: " << repro_error << std::endl;

  cv::FileStorage fs1(m_parametersSavePath + "/Intrinsics/Intrinsics_" + cameraName + ".yaml", cv::FileStorage::WRITE);
  fs1 << "intrinsicMatrix" << intrinsics.K.t();
  fs1 << "distortionCoefficients" << intrinsics.D;
}


void ExtrinsicsCalibrator::checkRotation(std::vector< cv::Point2f> &corners1, cv::Mat &img1) {
  int width = m_calibrationConfig->patternWidth;
  int height = m_calibrationConfig->patternHeight;
	cv::Point2i ctestd;
	cv::Point2f p1 = corners1[width*height-1];
	cv::Point2f p2 = corners1[width*height-2];
	cv::Point2f p3 = corners1[width*(height-1)-1];
	cv::Point2f p4 = corners1[width*(height-1)-2];
	ctestd.x = (p1.x + p2.x + p3.x + p4.x) / 4;
	ctestd.y = (p1.y + p2.y + p3.y + p4.y) / 4;

	cv::Point2i ctestl;
	p1 = corners1[0];
	p2 = corners1[1];
	p3 = corners1[width];
	p4 = corners1[width+1];
	ctestl.x = (p1.x + p2.x + p3.x + p4.x) / 4;
	ctestl.y = (p1.y + p2.y + p3.y + p4.y) / 4;

	cv::Vec3b colord = img1.at<cv::Vec3b>(ctestd.y,ctestd.x);
	int color_sum_d = colord[0]+colord[1]+colord[2];
	cv::Vec3b colorl = img1.at<cv::Vec3b>(ctestl.y,ctestl.x);
	int color_sum_l = colorl[0]+colorl[1]+colorl[2];

	if (color_sum_d > color_sum_l) {
		std::reverse(corners1.begin(),corners1.end());
	}
}


bool ExtrinsicsCalibrator::boardToCorners(cbdetect::Board &board, cbdetect::Corner &cbCorners, std::vector<cv::Point2f> &corners) {
  if (board.idx.size()-2 == 6) {
    for(int i = 1; i < board.idx.size() - 1; ++i) {
      for(int j = 1; j < board.idx[i].size() - 1; ++j) {
        if(board.idx[i][j] < 0) {
          return false;
        }
        corners.push_back(static_cast<cv::Point2f>(cbCorners.p[board.idx[i][j]]));
      }
    }
  }
  else {
    for(int j = 1; j < board.idx[0].size() - 1; ++j) {
      for(int i = 1; i < board.idx.size() - 1; ++i) {
        if(board.idx[board.idx.size() - 1 -i][j] < 0) {
          return false;
        }
        corners.push_back(static_cast<cv::Point2f>(cbCorners.p[board.idx[board.idx.size() - 1 -i][j]]));
      }
    }
  }
  return true;
}

bool ExtrinsicsCalibrator::tryLoadIntrinsics(QList<QString> cameraPair, Intrinsics &i1, Intrinsics &i2) {
  int numCameras = cameraPair.size();
  cv::FileStorage fsr1(m_parametersSavePath + "/Intrinsics/Intrinsics_" + cameraPair[0].toStdString() + ".yaml", cv::FileStorage::READ);
	cv::FileStorage fsr2(m_parametersSavePath + "/Intrinsics/Intrinsics_" + cameraPair[1].toStdString() + ".yaml", cv::FileStorage::READ);
  if (fsr1.isOpened() && fsr2.isOpened()) {
    fsr1["intrinsicMatrix"] >> i1.K;
    fsr1["distortionCoefficients"] >> i1.D;
    fsr2["intrinsicMatrix"] >> i2.K;
    fsr2["distortionCoefficients"] >> i2.D;
    i1.K = i1.K.t();
    i2.K = i2.K.t();
    return true;
  }
  return false;
}
