/*******************************************************************************
 * File:			  imagewriter.hpp
 * Created: 	  09. August 2021
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include "globals.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <QRunnable>


class ImageWriter : public QObject, public QRunnable {
	Q_OBJECT

	public:
		explicit ImageWriter(const QString &videoPath,
					const QString &destinationPath, QList<int> frameNumbers,
					int threadNumber);
		void run();

	signals:
		void copyImagesStatus(int frameCount, int totalNumFrames, int threadNumber);

	public slots:
		void creationCanceledSlot();

	private:
		cv::VideoCapture *m_cap;
		QString m_destinationPath;
		QList<int> m_frameNumbers;
		int m_threadNumber;
		int m_interrupt = false;
};

#endif
