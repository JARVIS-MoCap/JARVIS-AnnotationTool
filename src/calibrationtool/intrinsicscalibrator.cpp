/*******************************************************************************
 * File:			  intrinsicscalibrator.cpp
 * Created: 	  30. July 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "intrinsicscalibrator.hpp"
#include "colormap.hpp"


#include <sys/stat.h>
#include <sys/types.h>

#include <QThreadPool>
#include <QDir>


IntrinsicsCalibrator::IntrinsicsCalibrator(CalibrationConfig *calibrationConfig,
      const QString& cameraName, int threadNumber) :
      m_calibrationConfig(calibrationConfig),
      m_cameraName(cameraName.toStdString()), m_threadNumber(threadNumber){
  QDir dir;
  // dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
  //            m_calibrationConfig->calibrationSetName + "/Intrinsics");
  if (m_calibrationConfig->debug) {
    dir.mkpath(m_calibrationConfig->calibrationSetPath + "/" +
    m_calibrationConfig->calibrationSetName + "/debug/Intrinsics/" +
          cameraName + "/");
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


void IntrinsicsCalibrator::run() {
  if (m_calibrationConfig->boardType == "Standard" || m_calibrationConfig->boardType == "ChAruco") {
    run_standard();
  }
  else {
    run_charuco();
  }
}

void IntrinsicsCalibrator::run_standard() {
  std::vector<cv::Point3f> checkerBoardPoints;
  for (int i = 0; i < m_calibrationConfig->patternHeight; i++)
    for (int j = 0; j < m_calibrationConfig->patternWidth; j++)
      checkerBoardPoints.push_back(cv::Point3f((float)j *
            m_calibrationConfig->patternSideLength, (float)i *
            m_calibrationConfig->patternSideLength, 0));
  std::vector<std::string> fileNames;
  QString format = getFormat(m_calibrationConfig->intrinsicsPath,
                             QString::fromStdString(m_cameraName));

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
	int iteration = 0;
	int skipIndex;

	while (objectPointsAll.size() < m_calibrationConfig->framesForIntrinsics) {
		cv::VideoCapture cap(m_calibrationConfig->intrinsicsPath.toStdString() + "/" +
												 m_cameraName + "." + format.toStdString());
		int frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);

		if (iteration == 0) {
			skipIndex = std::max(1, frameCount/(m_calibrationConfig->framesForIntrinsics*2));
			skipIndex = skipIndex-skipIndex%4;
		}
		else if (iteration% 2 == 1 && iteration < 4) {
			cap.set(cv::CAP_PROP_POS_FRAMES, skipIndex/2);
		}
		else if (iteration == 2) {
			cap.set(cv::CAP_PROP_POS_FRAMES, skipIndex/4);
			skipIndex = skipIndex/2;
		}
		else if (iteration < 5) {
      imagePointsAll.clear();
      objectPointsAll.clear();
      cap.set(cv::CAP_PROP_POS_FRAMES, 0);
      skipIndex = 1;
		}
    else {
      break;
    }
		iteration++;

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
	      bool patternFound = (cbCorners.p.size() >=
	            m_calibrationConfig->patternHeight *
	            m_calibrationConfig->patternWidth);

	      if (patternFound) {
	        cbdetect::boards_from_corners(img, cbCorners, boards, params);
	        if(boards.size() == 1) {
	          patternFound = boardToCorners(boards[0], cbCorners, corners);
	        }
	        else {
	          patternFound = false;
	        }
	      }
	      if (patternFound && checkRotation(corners, img)) {
	        if (m_calibrationConfig->debug) {
	          saveCheckerboard(img, corners, counter);
	        }
	        imagePointsAll.push_back(corners);
	        objectPointsAll.push_back(checkerBoardPoints);
	      }
	      emit intrinsicsProgress(counter * (skipIndex + 1), frameCount,
	            m_threadNumber);
	      counter++;
	    }
	  }
		cap.release();
	  if (m_interrupt) return;
	}

  if (objectPointsAll.size() < m_calibrationConfig->framesForIntrinsics) {
      emit calibrationError("Camera " + QString::fromStdString(m_cameraName) +
      ": Found only " + QString::number(objectPointsAll.size()) +
      " valid checkerboards, aborting calibration. Make sure your checkerboard "
      "parameters are set correctly.");
      return;
  }

  double keep_ratio = imagePointsAll.size() /
        (double)std::min(m_calibrationConfig->framesForIntrinsics,
        (int)imagePointsAll.size());

  for (double k = 0; k < imagePointsAll.size(); k += keep_ratio) {
    imagePoints.push_back(imagePointsAll[(int)k]);
    objectPoints.push_back(objectPointsAll[(int)k]);
  }

  cv::Mat K, D;
  std::vector< cv::Mat > rvecs, tvecs;
  double repro_error = calibrateCamera(objectPoints, imagePoints, size,
    K, D, rvecs, tvecs,
    cv::CALIB_FIX_K3 | cv::CALIB_ZERO_TANGENT_DIST,
    cv::TermCriteria(cv::TermCriteria::MAX_ITER |
      cv::TermCriteria::EPS, 100, 1e-7));

  emit finishedIntrinsics(K, D, repro_error, m_threadNumber);
}



void IntrinsicsCalibrator::run_charuco() {
  std::vector<std::string> fileNames;
  QString format = getFormat(m_calibrationConfig->intrinsicsPath,
                             QString::fromStdString(m_cameraName));
  cv::Size size;
	int iteration = 0;
	int skipIndex;

  cv::Ptr<cv::aruco::Dictionary> dictionary =
    cv::aruco::getPredefinedDictionary(m_calibrationConfig->charucoPatternIdx);
  cv::Ptr<cv::aruco::CharucoBoard> board =
  cv::aruco::CharucoBoard::create(m_calibrationConfig->patternWidth,
  m_calibrationConfig->patternHeight, m_calibrationConfig->patternSideLength,
        m_calibrationConfig->markerSideLength, dictionary);
  cv::Ptr<cv::aruco::DetectorParameters> charucoParams =
  cv::aruco::DetectorParameters::create();

  cv::Mat imageCopy;
  cv::Mat boardImage;
  board->draw(cv::Size(600, 500), boardImage, 10, 1);
  cv::imwrite("/home/timo/Documents/JARVIS-AnnotationTool/BoardImage.jpg", boardImage);

  std::vector<std::vector<int>> charucoIdsAll, charucoIds;
  std::vector<std::vector<cv::Point2f>> charucoCornersAll, charucoCorners;


	while (charucoIdsAll.size() < m_calibrationConfig->framesForIntrinsics) {
		cv::VideoCapture cap(m_calibrationConfig->intrinsicsPath.toStdString() + "/" +
												 m_cameraName + "." + format.toStdString());
		int frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);


		if (iteration == 0) {
			skipIndex = std::max(1, frameCount/(m_calibrationConfig->framesForIntrinsics*2));
			skipIndex = skipIndex-skipIndex%4;
		}
		else if (iteration% 2 == 1 && iteration < 4) {
			cap.set(cv::CAP_PROP_POS_FRAMES, skipIndex/2);
		}
		else if (iteration == 2) {
			cap.set(cv::CAP_PROP_POS_FRAMES, skipIndex/4);
			skipIndex = skipIndex/2;
		}
    else if (iteration < 5) {
      charucoIdsAll.clear();
      charucoCornersAll.clear();
      cap.set(cv::CAP_PROP_POS_FRAMES, 0);
      skipIndex = 10;
		}
    else {
      break;
    }
		iteration++;

	  bool read_success = true;
	  int counter = 0;
	  cv::Mat img;
	  while (read_success && !m_interrupt) {
	    read_success = cap.read(img);
	    if (read_success) {
	      size = img.size();
	      int frameIndex = cap.get(cv::CAP_PROP_POS_FRAMES);
	      cap.set(cv::CAP_PROP_POS_FRAMES, frameIndex+skipIndex);
	      if (frameIndex > frameCount) read_success = false;
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners;
        img.copyTo(imageCopy);
        cv::aruco::detectMarkers(img, board->dictionary, markerCorners, markerIds, charucoParams);
         if (markerIds.size() > 5) {
             //cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
             std::vector<cv::Point2f> charucoCorners;
             std::vector<int> charucoIds;
             cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, img, board, charucoCorners, charucoIds);
         if (charucoIds.size() > 5) {
             cv::Scalar color = cv::Scalar(255, 0, 0);
             cv::aruco::drawDetectedCornersCharuco(imageCopy, charucoCorners, charucoIds, color);
             charucoCornersAll.push_back(charucoCorners);
             charucoIdsAll.push_back(charucoIds);
           }
        }
        cv::imwrite("/home/timo/Documents/JARVIS-AnnotationTool/temp/"+ m_cameraName + "/" + std::to_string(counter) + ".png" , imageCopy);
	      emit intrinsicsProgress(counter * (skipIndex + 1), frameCount,
	            m_threadNumber);
	      counter++;
	    }
	  }
		cap.release();
	  if (m_interrupt) return;
	}

  if (charucoIdsAll.size() < m_calibrationConfig->framesForIntrinsics) {
      emit calibrationError("Camera " + QString::fromStdString(m_cameraName) +
      ": Found only " + QString::number(charucoIdsAll.size()) +
      " valid checkerboards, aborting calibration. Make sure your checkerboard "
      "parameters are set correctly.");
      return;
  }

  double keep_ratio = charucoIdsAll.size() /
        (double)std::min(m_calibrationConfig->framesForIntrinsics,
        (int)charucoIdsAll.size());

  for (double k = 0; k < charucoIdsAll.size(); k += keep_ratio) {
    charucoIds.push_back(charucoIdsAll[(int)k]);
    charucoCorners.push_back(charucoCornersAll[(int)k]);
  }

  cv::Mat K, D;
  std::vector< cv::Mat > rvecs, tvecs;
  double repro_error = cv::aruco::calibrateCameraCharuco(charucoCornersAll,
        charucoIdsAll, board, size, K,
        D, rvecs, tvecs, cv::CALIB_FIX_K3 | cv::CALIB_ZERO_TANGENT_DIST,
        cv::TermCriteria(cv::TermCriteria::MAX_ITER |
        cv::TermCriteria::EPS, 100, 1e-7));

  //std::cout << "Repro Error: " << repro_error << std::endl;

  emit finishedIntrinsics(K, D, repro_error, m_threadNumber);
}

QString IntrinsicsCalibrator::getFormat(const QString& path,
        const QString& cameraName) {
  QString usedFormat;
	for (const auto& format : m_validRecordingFormats) {
		if (QFile::exists(path + "/" + cameraName  + "." + format)) {
      usedFormat = format;
    }
	}
	return usedFormat;
}


bool IntrinsicsCalibrator::checkRotation(std::vector< cv::Point2f> &corners1,
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
    // cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
    // cv::imwrite(m_parametersSavePath  + "test.jpg", imageCopy);

    m_detectedPattern = -1;
    for (int i = 0; i < markerCorners.size(); i++) {
      cv::Point2i markerPosition = getPositionOfMarkerOnBoard(corners1,
            markerCorners[i]);
      if (markerPosition.x != -1) {
        m_detectedPattern.at<int>(markerPosition.y+1,markerPosition.x+1) =
              markerIds[i];
      }
    }
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

int IntrinsicsCalibrator::matchPattern() {
  int unrotCount = 0;
  int rotCount = 0;
  for (int i = 0; i < m_calibrationConfig->patternWidth+1; i++) {
    for (int j = 0; j <  m_calibrationConfig->patternHeight+1; j++) {
      if (m_charucoPattern.at<int>(j,i) != -1) {
        if (m_charucoPattern.at<int>(j,i) == m_detectedPattern.at<int>(j,i) ||
              m_charucoPattern.at<int>(j,i) == m_detectedPattern.at<int>(j,i)) {
          unrotCount++;
        }
        if ((m_charucoPattern.at<int>(j,i) ==
              m_detectedPattern.at<int>(m_calibrationConfig->patternHeight-j,
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



cv::Point2i IntrinsicsCalibrator::getPositionOfMarkerOnBoard(
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


bool IntrinsicsCalibrator::boardToCorners(cbdetect::Board &board,
      cbdetect::Corner &cbCorners, std::vector<cv::Point2f> &corners) {
  if (board.idx.size()-2 == m_calibrationConfig->patternHeight) {
    for(int i = 1; i < board.idx.size() - 1; ++i) {
      if (board.idx[i].size()-2 == m_calibrationConfig->patternWidth) {
        for(int j = 1; j < board.idx[i].size() - 1; ++j) {
          if(board.idx[i][j] < 0 || board.idx[i][j] >= cbCorners.p.size()) {
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


void IntrinsicsCalibrator::calibrationCanceledSlot() {
  m_interrupt = true;
}


void IntrinsicsCalibrator::saveCheckerboard(const cv::Mat &img,
      const std::vector<cv::Point2f> &corners, int counter) {
  ColorMap *colorMap = new ColorMap(ColorMap::Jet);
  cv::Mat debug_img = img.clone();
  int index = 0;
  for (const auto & corner : corners) {
    QColor c = colorMap->getColor(index++, corners.size());
    cv::circle(debug_img, corner, 4, cv::Scalar(c.blue(), c.green(), c.red()),
          cv::FILLED, cv::LINE_8);
  }
  cv::imwrite(m_parametersSavePath + "/debug/Intrinsics/" + m_cameraName +
        "/Frame_" + QString::number(counter).toStdString() + ".jpg", debug_img);
}
