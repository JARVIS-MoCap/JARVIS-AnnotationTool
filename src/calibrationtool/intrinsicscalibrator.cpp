/*****************************************************************
	* File:			 intrinsicscalibrator.cpp
	* Created: 	 30. July 2021
	* Author:		 Timo Hueser
	* Contact: 	 timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "intrinsicscalibrator.hpp"
#include "colormap.hpp"


#include <sys/stat.h>
#include <sys/types.h>

#include <QThreadPool>
#include <QDir>


IntrinsicsCalibrator::IntrinsicsCalibrator(CalibrationConfig *calibrationConfig,
      const QString& cameraName, int threadNumber) :
      m_calibrationConfig(calibrationConfig), m_cameraName(cameraName.toStdString()), m_threadNumber(threadNumber){
  QDir dir;
  dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
             m_calibrationConfig->calibrationSetName + "/Intrinsics");
  if (m_calibrationConfig->debug) {
    dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
    m_calibrationConfig->calibrationSetName + "/debug/Intrinsics/" + cameraName + "/");
  }
  m_parametersSavePath = (m_calibrationConfig->calibrationSetPath + "/" +
                          m_calibrationConfig->calibrationSetName).toStdString();
}


void IntrinsicsCalibrator::run() {
  std::vector<cv::Point3f> checkerBoardPoints;
  for (int i = 0; i < m_calibrationConfig->patternHeight; i++)
    for (int j = 0; j < m_calibrationConfig->patternWidth; j++)
      checkerBoardPoints.push_back(cv::Point3f((float)j * m_calibrationConfig->patternSideLength,
                                   (float)i * m_calibrationConfig->patternSideLength, 0));
  std::vector<std::string> fileNames;
  QString format = getFormat(m_calibrationConfig->intrinsicsPath,
                             QString::fromStdString(m_cameraName));
  cv::VideoCapture cap(m_calibrationConfig->intrinsicsPath.toStdString() + "/" +
                       m_cameraName + "." + format.toStdString());
  int frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
  int frameRate = cap.get(cv::CAP_PROP_FPS);
  int skipIndex = std::max(frameRate/m_calibrationConfig->maxSamplingFrameRate-1,0);

  std::vector< std::vector< cv::Point3f > > objectPointsAll, objectPoints;
  std::vector< std::vector< cv::Point2f > > imagePointsAll, imagePoints;
  std::vector< cv::Point2f > corners;
  cv::Size size;
  cbdetect::Corner cbCorners;
  std::vector<cbdetect::Board> boards;
  cbdetect::Params params;
  params.corner_type = cbdetect::SaddlePoint;
  params.show_processing = false;
  params.show_debug_image = false;

  bool read_success = true;
  int counter = 0;
  cv::Mat img;
  while (read_success && !m_interrupt) {
    read_success = cap.read(img);
    if (read_success) {
      corners.clear();
      size = img.size();
      int frameIndex = cap.get(cv::CAP_PROP_POS_FRAMES);
      cap.set(cv::CAP_PROP_POS_FRAMES, frameIndex+skipIndex);
      if (frameIndex > frameCount) read_success = false;
      cbdetect::find_corners(img, cbCorners, params);
      bool patternFound = (cbCorners.p.size() >= m_calibrationConfig->patternHeight *
                           m_calibrationConfig->patternWidth);

      if (patternFound) {
        cbdetect::boards_from_corners(img, cbCorners, boards, params);
        patternFound = boardToCorners(boards[0], cbCorners, corners);
      }
      if (patternFound) {
        if (m_calibrationConfig->debug) {
          saveCheckerboard(img, corners, counter);
        }
        checkRotation(corners, img);
        imagePointsAll.push_back(corners);
        objectPointsAll.push_back(checkerBoardPoints);
      }
      emit intrinsicsProgress(counter*(skipIndex+1), frameCount, m_threadNumber);
      counter++;
    }
  }
  if (m_interrupt) return;

  if (objectPointsAll.size() < m_calibrationConfig->framesForIntrinsics) {
    emit calibrationError("Found " + QString::number(objectPointsAll.size()) +
                          " valid checkerboards. Make sure your checkerboard parameters " +
                          "are set correctly or specify a lower number of frames to use.");
    return;
  }

  double keep_ratio = imagePointsAll.size() / (double)std::min(m_calibrationConfig->framesForIntrinsics, (int)imagePointsAll.size());

  for (double k = 0; k < imagePointsAll.size(); k += keep_ratio) {
    imagePoints.push_back(imagePointsAll[(int)k]);
    objectPoints.push_back(objectPointsAll[(int)k]);
  }

  cv::Mat K, D;
  std::vector< cv::Mat > rvecs, tvecs;
  double repro_error = calibrateCamera(objectPoints, imagePoints, size, K, D, rvecs, tvecs,
    cv::CALIB_FIX_K3 | cv::CALIB_ZERO_TANGENT_DIST, cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 80, 1e-6));

  cv::FileStorage fs1(m_parametersSavePath + "/Intrinsics/Intrinsics_" + m_cameraName + ".yaml", cv::FileStorage::WRITE);
  fs1 << "intrinsicMatrix" << K.t();
  fs1 << "distortionCoefficients" << D;
  emit finishedIntrinsics(repro_error, m_threadNumber);
}


QString IntrinsicsCalibrator::getFormat(const QString& path, const QString& cameraName) {
  QString usedFormat;
	for (const auto& format : m_validRecordingFormats) {
		if (QFile::exists(path + "/" + cameraName  + "." + format)) usedFormat = format;
	}
	return usedFormat;
}


void IntrinsicsCalibrator::checkRotation(std::vector< cv::Point2f> &corners1, cv::Mat &img1) {
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


bool IntrinsicsCalibrator::boardToCorners(cbdetect::Board &board, cbdetect::Corner &cbCorners,
      std::vector<cv::Point2f> &corners) {
  if (board.idx.size()-2 == m_calibrationConfig->patternHeight) {
    for(int i = 1; i < board.idx.size() - 1; ++i) {
      if (board.idx[i].size()-2 == m_calibrationConfig->patternWidth) {
        for(int j = 1; j < board.idx[i].size() - 1; ++j) {
          if(board.idx[i][j] < 0 || board.idx[i][j] >= cbCorners.p.size()) {
            return false;
          }
          corners.push_back(static_cast<cv::Point2f>(cbCorners.p[board.idx[i][j]]));
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
          corners.push_back(static_cast<cv::Point2f>(cbCorners.p[board.idx[board.idx.size() - 1 -i][j]]));
        }
        else {
          return false;
        }
      }
    }
  }
  return true;
}


void IntrinsicsCalibrator::calibrationCanceledSlot() {
  m_interrupt = true;
}


void IntrinsicsCalibrator::saveCheckerboard(const cv::Mat &img, const std::vector<cv::Point2f> &corners, int counter) {
  ColorMap *colorMap = new ColorMap(ColorMap::Jet);
  cv::Mat debug_img = img.clone();
  int index = 0;
  for (const auto & corner : corners) {
    QColor c = colorMap->getColor(index++, corners.size());
    cv::circle(debug_img, corner, 4, cv::Scalar(c.blue(), c.green(), c.red()), cv::FILLED, cv::LINE_8);
  }
  cv::imwrite(m_parametersSavePath + "/debug/Intrinsics/" + m_cameraName + "/Frame_" + QString::number(counter).toStdString() + ".jpg", debug_img);
}
