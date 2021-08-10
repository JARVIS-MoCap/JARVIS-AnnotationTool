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
			cv::imwrite((m_destinationPath + "/" +  "Frame_" + QString::number(frameNumber) + ".jpg").toStdString(), frame);
			frameCount++;
		}
		else {
			std::cout << "Error reading Frame"<< std::endl;
			return;
		}
		if (m_threadNumber == 0 && frameCount % 1 == 0) {
			emit copyImagesStatus(frameCount, totalNumFrames);
		}
		if (m_interrupt) {
			std::cout << "JABSFKBAFKJFAB" << std::endl;
			return;
		}
	}
}

void ImageWriter::creationCanceledSlot() {
	std::cout << "caaaancle" << std::endl;
	m_interrupt = true;
}
