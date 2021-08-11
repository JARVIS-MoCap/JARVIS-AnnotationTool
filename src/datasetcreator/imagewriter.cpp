/*------------------------------------------------------------
 *  imagewriter.cpp
 *  Created:  10. July 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "imagewriter.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QErrorMessage>
#include <QDirIterator>
#include <QThreadPool>

#include <chrono>
using namespace std::chrono;

ImageWriter::ImageWriter(const QString &videoPath, const QString &destinationPath, QList<int> frameNumbers, int threadNumber) :
	m_destinationPath(destinationPath), m_frameNumbers(frameNumbers), m_threadNumber(threadNumber) {
		m_cap = new cv::VideoCapture(videoPath.toStdString());
}

void ImageWriter::run() {
	int totalNumFrames = m_frameNumbers.size();
	int frameCount = 0;

	for (const auto & frameNumber : m_frameNumbers) {
		cv::Mat frame;
		m_cap->set(cv::CAP_PROP_POS_FRAMES, frameNumber-1);
		if (m_cap->read(frame)) {
			//std::cout << frame.size() << std::endl;
			//std::cout << (m_destinationPath + "/" +  "Frame_" + QString::number(frameNumber) + ".jpg").toStdString() << std::endl;
			std::cout << cv::imwrite((m_destinationPath + "/" +  "Frame_" + QString::number(frameNumber) + ".jpg").toStdString(), frame) << std::endl;
			frameCount++;
		}
		else {
			std::cout << "Error reading Frame"<< std::endl;
			m_cap->release();
			return;
		}
		emit copyImagesStatus(frameCount, totalNumFrames, m_threadNumber);
		if (m_interrupt) {
			m_cap->release();
			return;
		}
	}
	m_cap->release();
}

void ImageWriter::creationCanceledSlot() {
	m_interrupt = true;
}
