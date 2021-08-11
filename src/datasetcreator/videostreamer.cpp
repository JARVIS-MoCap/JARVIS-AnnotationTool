/*****************************************************************
* File:			  videostreamer.cpp
* Created: 	  09. August 2021
* Author:		  Timo Hueser
* Contact: 	  timo.hueser@gmail.com
* Copyright:  2021 Timo Hueser
* License:    GPL v3.0
*****************************************************************/

#include "videostreamer.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QErrorMessage>
#include <QDirIterator>
#include <QThreadPool>


VideoStreamer::VideoStreamer(const QString &videoPath, QList<TimeLineWindow> timeLineWindows,
			int numFramesToExtract, int threadNumber) {
	m_cap = new cv::VideoCapture(videoPath.toStdString());
	m_threadNumber = threadNumber;
	m_timeLineWindows = timeLineWindows;
	m_numFramesToExtract = numFramesToExtract;
}


void VideoStreamer::run() {
	bool readFrame = true;
	int frameCount = 0;
	int indexCount = 0;
	int totalFrames = 0;
	int minFrameCount = 0;
	int subSamplingRate = 10;
	cv::Mat img,dctImage;
	for (const auto & window : m_timeLineWindows) {
		int windowSize = window.end-window.start;
		totalFrames += windowSize;
		if (minFrameCount == 0 || windowSize < minFrameCount) minFrameCount = windowSize;
	}
	bool zeroReached = false;
	while (m_numFramesToExtract*4 > minFrameCount/subSamplingRate && !zeroReached) {
		subSamplingRate = subSamplingRate/2;
		if (subSamplingRate == 0) {
			zeroReached = true;
			subSamplingRate = 1;
		}
	}

	for (const auto & window : m_timeLineWindows) {
		frameCount = window.start;
		while (frameCount < window.end) {
			emit dctProgress(indexCount, totalFrames/subSamplingRate, m_threadNumber);
			m_cap->set(cv::CAP_PROP_POS_FRAMES, frameCount);
			readFrame = m_cap->read(img);
			if (readFrame) {
				cv::resize(img, img, cv::Size(160, 128));
				cv::cvtColor(img,img, cv::COLOR_BGR2GRAY);
				img.convertTo(img, CV_32FC1);
				cv::dct(img/255.0, dctImage);
				dctImage = dctImage(cv::Rect(0,0,20,16));
				m_dctImages.append(dctImage);
				m_frameNumberMap[indexCount++] = frameCount;
				frameCount += subSamplingRate;
			}
			if (m_interrupt) {
				m_cap->release();
				return;
			}
		}
	}
	m_cap->release();
	emit computedDCTs(m_dctImages, m_frameNumberMap, m_threadNumber);
}


void VideoStreamer::creationCanceledSlot() {
	m_interrupt = true;
}
