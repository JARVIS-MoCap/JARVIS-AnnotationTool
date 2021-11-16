/*******************************************************************************
 * File:			  extrinsicscalibrator.hpp
 * Created: 	  30. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2021 Timo Hueser
 * License:     LGPL v3.0
 ******************************************************************************/

#include "extrinsicscalibrator.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include <QThreadPool>
#include <QDir>


ExtrinsicsCalibrator::ExtrinsicsCalibrator(CalibrationConfig *calibrationConfig,
      QList<QString> cameraPair, int threadNumber) :
      m_calibrationConfig(calibrationConfig), m_cameraPair(cameraPair),
      m_threadNumber(threadNumber) {
  QDir dir;
  dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
             m_calibrationConfig->calibrationSetName + "/Intrinsics");
  dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
             m_calibrationConfig->calibrationSetName + "/Extrinsics");
  if (m_calibrationConfig->debug) {
    dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
    m_calibrationConfig->calibrationSetName + "/debug/Extrinsics/" +
          cameraPair[0] + "_" + cameraPair[1] + "/");
    if (cameraPair.size() == 3) {
      dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
      m_calibrationConfig->calibrationSetName + "/debug/Extrinsics/" +
            cameraPair[1] + "_" + cameraPair[2] + "/");
    }

  }
  m_parametersSavePath = (m_calibrationConfig->calibrationSetPath + "/" +
        m_calibrationConfig->calibrationSetName).toStdString();

  m_charucoPattern = cv::Mat(cv::Size( m_calibrationConfig->patternWidth+1,
        m_calibrationConfig->patternHeight+1), CV_32SC1);
  m_detectedPattern = cv::Mat(cv::Size( m_calibrationConfig->patternWidth+1,
        m_calibrationConfig->patternHeight+1), CV_32SC1);
  m_charucoPattern = -1;
  int id_count = 0;
  for (int i = 0; i < m_calibrationConfig->patternWidth+1; i++) {
    for (int j = 0; j <  m_calibrationConfig->patternHeight+1; j++) {
      if ((i+j)%2 != 0) {
        m_charucoPattern.at<int>(j,i) = id_count;
        id_count++;
      }
    }
  }
}


void ExtrinsicsCalibrator::run() {
  int numCameras = m_cameraPair.size();
  Extrinsics extrinsics1, extrinsics2;

  if (numCameras == 2) {
    if (!calibrateExtrinsicsPair(m_cameraPair, extrinsics1)) {
      return;
    }
    if (m_interrupt) return;
    cv::FileStorage fse(m_parametersSavePath + "/Extrinsics/Extrinsics_" +
          m_cameraPair[0].toStdString() + "_" + m_cameraPair[1].toStdString() +
          ".yaml", cv::FileStorage::WRITE);
    fse << "R" << extrinsics1.R.t();
    fse << "T" << extrinsics1.T;
    fse << "E" << extrinsics1.E;
    fse << "F" << extrinsics1.F;
  }

  else if (numCameras == 3) {
    QList<QString> cameraPair1 = {m_cameraPair[0], m_cameraPair[1]};
    QList<QString> cameraPair2 = {m_cameraPair[1], m_cameraPair[2]};
    bool success1 = calibrateExtrinsicsPair(cameraPair1, extrinsics1);
    bool success2 = calibrateExtrinsicsPair(cameraPair2, extrinsics2);
    if (!success1 || !success2) {
      return;
    }
    if (m_interrupt) return;
    cv::Mat T1_t = extrinsics1.R.t() * extrinsics1.T;
    cv::Mat T2_t = extrinsics2.R.t() * extrinsics2.T;
    cv::Mat R = extrinsics2.R*extrinsics1.R;
    cv::Mat T = R*(T1_t + extrinsics1.R.t()*T2_t);
    cv::FileStorage fse(m_parametersSavePath + "/Extrinsics/Extrinsics_" +
          m_cameraPair[0].toStdString() + "_" + m_cameraPair[2].toStdString() +
          ".yaml", cv::FileStorage::WRITE);
    fse << "R" << R.t();
    fse << "T" << T;
    fse << "E" << extrinsics1.E;
    fse << "F" << extrinsics1.F;
  }
}


bool ExtrinsicsCalibrator::calibrateExtrinsicsPair(QList<QString> cameraPair,
      Extrinsics &e) {
  std::vector<cv::Point3f> checkerBoardPoints;
  for (int i = 0; i < m_calibrationConfig->patternHeight; i++)
    for (int j = 0; j < m_calibrationConfig->patternWidth; j++)
      checkerBoardPoints.push_back(cv::Point3f((float)j *
            m_calibrationConfig->patternSideLength,
            (float)i * m_calibrationConfig->patternSideLength, 0));

  std::string cap1Path;
  std::string cap2Path;
  if (m_calibrationConfig->single_primary == false) {
    QString format1 = getFormat(m_calibrationConfig->extrinsicsPath + "/" +
          cameraPair[0] + "-" + cameraPair[1], cameraPair[0]);
    QString format2 = getFormat(m_calibrationConfig->extrinsicsPath + "/" +
          cameraPair[0] + "-" + cameraPair[1], cameraPair[1]);
    cap1Path = (m_calibrationConfig->extrinsicsPath + "/" + cameraPair[0] +
          "-" + cameraPair[1] + "/" + cameraPair[0] + "." +
          format1).toStdString();
    cap2Path = (m_calibrationConfig->extrinsicsPath + "/" + cameraPair[0] +
          "-" + cameraPair[1] + "/" + cameraPair[1] + "." +
          format1).toStdString();
  }
  else {
    QString format1 = getFormat(m_calibrationConfig->extrinsicsPath,
          cameraPair[0]);
    QString format2 = getFormat(m_calibrationConfig->extrinsicsPath,
          cameraPair[1]);
    cap1Path = (m_calibrationConfig->extrinsicsPath + "/" + cameraPair[0] +
          "." + format1).toStdString();
    cap2Path = (m_calibrationConfig->extrinsicsPath + "/" + cameraPair[1] +
          "." + format2).toStdString();
  }
  cv::VideoCapture cap1(cap1Path);
  cv::VideoCapture cap2(cap2Path);

  int frameCount = cap1.get(cv::CAP_PROP_FRAME_COUNT);
  int frameRate = cap1.get(cv::CAP_PROP_FPS);
  int skipIndex = std::max(frameRate /
        m_calibrationConfig->maxSamplingFrameRate-1,0);

  std::vector<std::vector<cv::Point3f>> objectPointsAll, objectPoints;
  std::vector<std::vector<cv::Point2f>> imagePointsAll1, imagePointsAll2,
                                        imagePoints1, imagePoints2;
  std::vector<cv::Point2f> corners1, corners2;
	cv::Size size;
  std::vector<cbdetect::Board> boards1, boards2;
  cbdetect::Params params;
  params.corner_type = cbdetect::SaddlePoint;
  params.show_processing = false;
  params.show_debug_image = false;

  bool read_success = true;
  int counter = 0;
  cv::Mat img1,img2;
  while (read_success && !m_interrupt) {
    bool read_success1 = cap1.read(img1);
    bool read_success2 = cap2.read(img2);
    read_success = read_success1 && read_success2;
    if (read_success) {
      int frameIndex = cap1.get(cv::CAP_PROP_POS_FRAMES);
      cap1.set(cv::CAP_PROP_POS_FRAMES, frameIndex+skipIndex);
      cap2.set(cv::CAP_PROP_POS_FRAMES, frameIndex+skipIndex);
      if (frameIndex > frameCount) read_success = false;
      corners1.clear();
      corners2.clear();
      size = img1.size();

      cbdetect::Corner cbCorners1, cbCorners2;
      cbdetect::find_corners(img1, cbCorners1, params);
      cbdetect::find_corners(img2, cbCorners2, params);

      bool patternFound1 = (cbCorners1.p.size() >=
                            m_calibrationConfig->patternHeight *
                            m_calibrationConfig->patternWidth);
      bool patternFound2 = (cbCorners2.p.size() >=
                            m_calibrationConfig->patternHeight *
                            m_calibrationConfig->patternWidth);

      if (patternFound1 && patternFound2) {
        cbdetect::boards_from_corners(img1, cbCorners1, boards1, params);
        cbdetect::boards_from_corners(img2, cbCorners2, boards2, params);
        if (boards1.size() == 1) {
          patternFound1 = boardToCorners(boards1[0], cbCorners1, corners1);
        }
        else {
          patternFound1 = false;
        }
        if (boards2.size() == 1) {
          patternFound2 = boardToCorners(boards2[0], cbCorners2, corners2);
        }
        else {
          patternFound2 = false;
        }
        if (patternFound1 && patternFound2 && checkRotation(corners1, img1) &&
              checkRotation(corners2, img2)) {
          if (m_calibrationConfig->debug) {
            saveCheckerboard(cameraPair, img1,img2,corners1,corners2,counter);
          }
          imagePointsAll1.push_back(corners1);
          imagePointsAll2.push_back(corners2);
          objectPointsAll.push_back(checkerBoardPoints);
        }
      }
      emit extrinsicsProgress(counter*(skipIndex+1), frameCount,
            m_threadNumber);
      counter++;
    }
  }
  if (m_interrupt) return false;

  if(objectPointsAll.size() < 15) {
    emit calibrationError("Found only " +
          QString::number(objectPointsAll.size()) + " valid checkerboards, "
          "aborting calibration. Make sure your checkerboard parameters are "
          "set correctly.");
    return false;
  }

  double keep_ratio = imagePointsAll1.size() /
        (double)std::min(m_calibrationConfig->framesForExtrinsics,
        (int)imagePointsAll1.size());
  for (double k = 0; k < imagePointsAll1.size(); k += keep_ratio) {
    imagePoints1.push_back(imagePointsAll1[(int)k]);
    imagePoints2.push_back(imagePointsAll2[(int)k]);
    objectPoints.push_back(objectPointsAll[(int)k]);
  }

  Intrinsics i1,i2;
  QMap<QString, double> intrinsicsErrorMap;
  bool intrinsicsFound = tryLoadIntrinsics(cameraPair, i1,i2);
  if (!intrinsicsFound) {
    intrinsicsErrorMap[cameraPair[0]] = calibrateIntrinsicsStep(
          cameraPair[0].toStdString(), objectPoints, imagePoints1, size, i1);
    intrinsicsErrorMap[cameraPair[1]] = calibrateIntrinsicsStep(
          cameraPair[1].toStdString(), objectPoints, imagePoints2, size, i2);
  }
  else {
    std::cout << "FOUND INTRINSICS" << std::endl;
  }

  double mean_repro_error = stereoCalibrationStep(objectPoints, imagePoints1,
        imagePoints2, i1,i2,e, size, 1.4);
  std::cout << "Mean Reprojection Error after Stage 1: " <<
        mean_repro_error << std::endl;
  std::cout << "Number Images for Stage 2: " <<
        imagePoints1.size() << std::endl;

  mean_repro_error = stereoCalibrationStep(objectPoints, imagePoints1,
        imagePoints2, i1,i2,e, size, 1.6);
  std::cout << "Mean Reprojection Error after Stage 2: " <<
        mean_repro_error << std::endl;
  std::cout << "Final Number Images: " <<imagePoints1.size() << std::endl;

  cv::Mat errs;
  mean_repro_error = cv::stereoCalibrate(objectPoints, imagePoints1,
        imagePoints2, i1.K, i1.D, i2.K, i2.D, size, e.R, e.T, e.E, e.F,errs,
        cv::CALIB_FIX_INTRINSIC, cv::TermCriteria(cv::TermCriteria::MAX_ITER |
        cv::TermCriteria::EPS, 120, 1e-7));

  emit finishedExtrinsics(mean_repro_error, intrinsicsErrorMap, m_threadNumber);
  //TODO: this is not quite right for triplet, do average over both instead or something
  return true;

}


double ExtrinsicsCalibrator::stereoCalibrationStep(
      std::vector<std::vector<cv::Point3f>> &objectPoints,
      std::vector<std::vector<cv::Point2f>> &imagePoints1,
      std::vector<std::vector<cv::Point2f>> &imagePoints2,
      Intrinsics &i1, Intrinsics &i2, Extrinsics &e,
      cv::Size size, double thresholdFactor) {
  cv::Mat errs;

  double mean_repro_error  = cv::stereoCalibrate(objectPoints,
        imagePoints1, imagePoints2, i1.K, i1.D, i2.K, i2.D, size, e.R, e.T,
        e.E, e.F,errs, cv::CALIB_FIX_INTRINSIC,
        cv::TermCriteria(cv::TermCriteria::MAX_ITER |
        cv::TermCriteria::EPS, 80, 1e-6));

  std::vector< std::vector< cv::Point3f > > objectPoints_2;
  std::vector< std::vector< cv::Point2f > > imagePoints1_2, imagePoints2_2;
  for (int i = 0; i < errs.size[0]; i++) {
    if (std::max(errs.at<double>(i,0), errs.at<double>(i,1)) <
          thresholdFactor * mean_repro_error) {
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


double ExtrinsicsCalibrator::calibrateIntrinsicsStep(std::string cameraName,
      std::vector<std::vector<cv::Point3f>> objectPoints,
      std::vector<std::vector<cv::Point2f>> imagePoints, cv::Size size,
      Intrinsics &intrinsics) {

  std::vector< cv::Mat > rvecs, tvecs;

  double repro_error = calibrateCamera(objectPoints, imagePoints, size,
        intrinsics.K, intrinsics.D, rvecs, tvecs,
        cv::CALIB_FIX_K3 | cv::CALIB_ZERO_TANGENT_DIST,
        cv::TermCriteria(cv::TermCriteria::MAX_ITER |
        cv::TermCriteria::EPS, 80, 1e-6));

  cv::FileStorage fs1(m_parametersSavePath + "/Intrinsics/Intrinsics_" +
        cameraName + ".yaml", cv::FileStorage::WRITE);
  fs1 << "intrinsicMatrix" << intrinsics.K.t();
  fs1 << "distortionCoefficients" << intrinsics.D;

  return repro_error;
}


QString ExtrinsicsCalibrator::getFormat(const QString& path,
      const QString& cameraName) {
  QString usedFormat;
	for (const auto& format : m_validRecordingFormats) {
		if (QFile::exists(path + "/" + cameraName  + "." + format)) {
      usedFormat = format;
    }
	}
	return usedFormat;
}



bool ExtrinsicsCalibrator::checkRotation(std::vector< cv::Point2f> &corners1,
      cv::Mat &img1) {
  if (m_calibrationConfig->boardType == "Standard") {
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
    return true;
  }
  else {
    cv::Ptr<cv::aruco::Dictionary> dictionary =
          cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50);
    cv::Ptr<cv::aruco::CharucoBoard> board =
          cv::aruco::CharucoBoard::create(7, 5, 0.04f, 0.02f, dictionary);
    cv::Ptr<cv::aruco::DetectorParameters> params =
          cv::aruco::DetectorParameters::create();

    params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR;
    cv::Mat  imageCopy;
    img1.copyTo(imageCopy);
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f> > markerCorners;
    cv::aruco::detectMarkers(img1, board->dictionary, markerCorners, markerIds,
          params);
    if (markerIds.size() == 0) return false;
    cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
    cv::imwrite(m_parametersSavePath  + "test.jpg", imageCopy);

    m_detectedPattern = -1;
    for (int i = 0; i < markerCorners.size(); i++) {
      cv::Point2i markerPosition = getPositionOfMarkerOnBoard(corners1,
            markerCorners[i]);
      if (markerPosition.x != -1) {
        m_detectedPattern.at<int>(markerPosition.y+1,markerPosition.x+1) =
              markerIds[i];
      }
    }
    std::cout << m_detectedPattern << std::endl;
    int match = matchPattern();
    if (match == 0) {
      return false;
    }
    else if (match == 2) {
      std::reverse(corners1.begin(),corners1.end());
    }

    return true;
  }
}

int ExtrinsicsCalibrator::matchPattern() {
  int unrotCount = 0;
  int rotCount = 0;
  for (int i = 0; i < m_calibrationConfig->patternWidth+1; i++) {
    for (int j = 0; j <  m_calibrationConfig->patternHeight+1; j++) {
      if (m_charucoPattern.at<int>(j,i) != -1) {
        if (m_charucoPattern.at<int>(j,i) == m_detectedPattern.at<int>(j,i) ||
              m_charucoPattern.at<int>(j,i) == m_detectedPattern.at<int>(j,i)) {
          unrotCount++;
        }
        if ((m_charucoPattern.at<int>(j,i) == m_detectedPattern.at<int>(
              m_calibrationConfig->patternHeight-j,
              m_calibrationConfig->patternWidth-i))) {
          rotCount++;
        }
      }
    }
  }
  if (unrotCount == rotCount) {
    return 0;
  }
  else if (unrotCount > rotCount) {
    return 1;
  }
  else if (unrotCount < rotCount) {
    return 2;
  }
}



cv::Point2i ExtrinsicsCalibrator::getPositionOfMarkerOnBoard(
      std::vector< cv::Point2f>&cornersBoard,
      std::vector<cv::Point2f>&markerCorners) {
  int width = m_calibrationConfig->patternWidth;
  int height = m_calibrationConfig->patternHeight;
  cv::Point2i position;
  position.x = -1;
  position.y = -1;
  for (int i = 0; i < width-1; i++) {
    for (int j = 0; j < height-1; j++) {
      cv::Point2f markerCenter;
      markerCenter.x = (markerCorners[0].x+markerCorners[2].x)/2;
      markerCenter.y = (markerCorners[0].y+markerCorners[2].y)/2;
      cv::Point2f p1 = cornersBoard[j*width+i];
      cv::Point2f p2 = cornersBoard[(j+1)*width+(i+1)];

      if (((p1.x < p2.x && markerCenter.x > p1.x && markerCenter.x < p2.x) ||
            (p1.x > p2.x && markerCenter.x < p1.x && markerCenter.x > p2.x))  &&
            ((p1.y < p2.y && markerCenter.y > p1.y && markerCenter.y < p2.y) ||
            (p1.y > p2.y && markerCenter.y < p1.y && markerCenter.y > p2.y))) {
        position.x = i;
        position.y = m_calibrationConfig->patternHeight-2-j;
      }
    }
  }
  return position;
}


bool ExtrinsicsCalibrator::boardToCorners(cbdetect::Board &board,
      cbdetect::Corner &cbCorners, std::vector<cv::Point2f> &corners) {
  if (board.idx.size()-2 == m_calibrationConfig->patternHeight) {
    for(int i = 1; i < board.idx.size() - 1; ++i) {
      if (board.idx[i].size()-2 == m_calibrationConfig->patternWidth) {
        for(int j = 1; j < board.idx[i].size() - 1; ++j) {
          if(board.idx[i][j] < 0) {
            return false;
          }
          if (board.idx[i][j] >= cbCorners.p.size()) {
            return false;
          }
          corners.push_back(static_cast<cv::Point2f>(
                cbCorners.p[board.idx[i][j]]));
        }
      }
      else {
        return false;
      }
    }
  }
  else {
    for(int j = 1; j < board.idx[0].size() - 1; ++j) {
      for(int i = 1; i < board.idx.size() - 1; ++i) {
        if (board.idx.size()-2 == m_calibrationConfig->patternWidth &&
            board.idx[i].size()-2 == m_calibrationConfig->patternHeight) {
          if(board.idx[board.idx.size() - 1 -i][j] < 0) {
            return false;
          }
          if (board.idx[board.idx.size() - 1 -i][j] >= cbCorners.p.size()) {
            return false;
          }
          corners.push_back(static_cast<cv::Point2f>(
                cbCorners.p[board.idx[board.idx.size() - 1 -i][j]]));
        }
        else {
          return false;
        }
      }
    }
  }
  return true;
}


bool ExtrinsicsCalibrator::tryLoadIntrinsics(QList<QString> cameraPair,
      Intrinsics &i1, Intrinsics &i2) {
  cv::FileStorage fsr1(m_parametersSavePath + "/Intrinsics/Intrinsics_" +
        cameraPair[0].toStdString() + ".yaml", cv::FileStorage::READ);
	cv::FileStorage fsr2(m_parametersSavePath + "/Intrinsics/Intrinsics_" +
        cameraPair[1].toStdString() + ".yaml", cv::FileStorage::READ);

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


void ExtrinsicsCalibrator::calibrationCanceledSlot() {
  m_interrupt = true;
}


void ExtrinsicsCalibrator::saveCheckerboard(QList<QString> cameraPair,
      const cv::Mat &img1, const cv::Mat &img2,
      const std::vector<cv::Point2f> &corners1,
      const std::vector<cv::Point2f> &corners2, int counter) {
  ColorMap *colorMap = new ColorMap(ColorMap::Jet);
  cv::Mat debug_img1 = img1.clone();
  cv::Mat debug_img2 = img2.clone();
  cv::Mat debug_img;
  int index = 0;
  for (const auto & corner : corners1) {
    QColor c = colorMap->getColor(index++, corners1.size());
    cv::circle(debug_img1, corner, 4, cv::Scalar(c.blue(), c.green(), c.red()),
          cv::FILLED, cv::LINE_8);
  }
  index = 0;
  for (const auto & corner : corners2) {
    QColor c = colorMap->getColor(index++, corners2.size());
    cv::circle(debug_img2, corner, 4, cv::Scalar(c.blue(), c.green(), c.red()),
          cv::FILLED, cv::LINE_8);
  }
  cv::resize(debug_img2, debug_img2, debug_img1.size());
  cv::Mat matArray[] = {debug_img1, debug_img2};
  std::cout << debug_img1.size() << ", " << debug_img2.size() << std::endl;
  cv::hconcat(matArray, 2, debug_img);
  cv::imwrite(m_parametersSavePath + "/debug/Extrinsics/" +
        cameraPair[0].toStdString() + "_" + cameraPair[1].toStdString() +
        "/Frame_" + QString::number(counter).toStdString() + ".jpg", debug_img);
}
