/*------------------------------------------------------------
 *  imagewriter.hpp
 *  Created: 09. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

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
		explicit ImageWriter(const QString &videoPath, const QString &destinationPath, QList<int> frameNumbers, int threadNumber);
		void run();

	public slots:
		void creationCanceledSlot();

	signals:
		void copyImagesStatus(int frameCount, int totalNumFrames, int threadNumber);

	private:
		cv::VideoCapture *m_cap;
		int m_threadNumber;
		QString m_destinationPath;
		QList<int> m_frameNumbers;
		int m_interrupt = false;
};

#endif
