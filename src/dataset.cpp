/*------------------------------------------------------------
 *  dataset.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "dataset.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QErrorMessage>

Dataset * Dataset::dataset = nullptr;

Dataset::Dataset(const QString& datasetFolder, QList<QString> cameraNames) : m_datasetFolder(datasetFolder) {
	m_colorMap = new ColorMap(ColorMap::Jet);
	if (cameraNames.size() == 0) {
		m_cameraNames = QDir(datasetFolder).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	}
	else {
		m_cameraNames = cameraNames;
	}
	m_numCameras = m_cameraNames.size();
	if (m_numCameras == 0) {
		QErrorMessage *msg = new QErrorMessage();
		msg->showMessage("This directory does not contain any subdirectories! Make sure the savefiles for each camera are stored in a seperate subdirectory (even if just one camera is used).");
		return;
	}
	QList<QFile*> saveFiles;
	for (int i = 0; i < m_numCameras; i++) {
		QFile *file = new QFile(datasetFolder + "/" + m_cameraNames[i] +"/annotations.csv");
		saveFiles.append(file);
		if (!file->open(QIODevice::ReadOnly)) {
			QErrorMessage *msg = new QErrorMessage();
			msg->showMessage("Error reading savefile for Camera " + m_cameraNames[i] + "! Make sure a savefile called 'annotations.csv' exists in the cameras directory.");
			return;
		}
	}
	int lineCount = 0;
	m_scorer = saveFiles[0]->readLine().split(',')[1];
	QList<QByteArray> cells = saveFiles[0]->readLine().split(',');
	for (int i = 1; i < cells.size(); i+= 3) {
		m_entityNameList.append(cells[i]);
		if(!m_entitiesList.contains(cells[i])) m_entitiesList.append(cells[i]);
	}
	cells = saveFiles[0]->readLine().split(',');
	for (int i = 1; i < cells.size(); i+= 3) {
		m_keypointNameList.append(cells[i]);
		if (!m_bodypartsList.contains(cells[i])) m_bodypartsList.append(cells[i]);
	}
	saveFiles[0]->readLine();
	for (int i = 1; i < m_numCameras; i++) {
		saveFiles[i]->readLine();
		saveFiles[i]->readLine();
		saveFiles[i]->readLine();
		saveFiles[i]->readLine();
	}
	while (!saveFiles[0]->atEnd()) {
		ImgSet *imgSet = new ImgSet();
		imgSet->numCameras = m_numCameras;
		for (int cam = 0; cam < m_numCameras; cam++) {
			cells = saveFiles[cam]->readLine().split(',');
			Frame *frame = new Frame();
			frame->imagePath = datasetFolder + "/" + m_cameraNames[cam] + "/" + cells[0];
			int x,y;
			GetImageSizeEx(frame->imagePath, &x,&y);
			frame->imageDimensions = QSize(x,y);
			frame->numKeypoints = m_keypointNameList.size();
			for (int i = 0; i < m_keypointNameList.size(); i++) {
				QColor color = m_colorMap->getColor(i%m_bodypartsList.size(), m_bodypartsList.size());
				Keypoint *keypoint = new Keypoint(m_entityNameList[i], m_keypointNameList[i], color, QPointF(cells[3*i+1].toFloat(), cells[3*i+2].toFloat()));
				keypoint->setFrameIndex(cam);
				if (cells[3*i+3].toInt() == 0) {
					keypoint->setState(NotAnnotated);
				}
				else if (cells[3*i+3].toInt() == 1) {
					keypoint->setState(Annotated);
				}
				else if (cells[3*i+3].toInt() == 2) {
					//keypoint->setState(Reprojected);
					keypoint->setState(NotAnnotated);
				}
				else if (cells[3*i+3].toInt() == 3) {
					keypoint->setState(Suppressed);
				}
				connect(keypoint, &Keypoint::stateChanged, this, &Dataset::keypointStateChanged);
				frame->keypoints.append(keypoint);
				frame->keypointMap[keypoint->ID()] = keypoint;
			}
			imgSet->frames.append(frame);
		}
		lineCount += 1;
		m_imgSets.append(imgSet);
  }
	for (int i = 0; i < m_numCameras; i++) {
		saveFiles[i]->close();
	}
	m_loadSuccessfull = true;
}

void Dataset::save(const QString& datasetFolder) {
	QString dataFolder = datasetFolder;
	if (datasetFolder == "") dataFolder = m_datasetFolder;
	QList<QFile*> saveFiles;
	for (int i = 0; i < m_numCameras; i++) {
		QFile *file = new QFile(dataFolder + "/" + m_cameraNames[i] +"/annotations.csv");
		saveFiles.append(file);
		if (!file->open(QIODevice::WriteOnly)) {
			std::cout << "Can't open File" << std::endl;
			QErrorMessage *msg = new QErrorMessage();
			msg->showMessage("Error writing savefile. Make sure you have the right permissions...");
			return;
		}
		 QTextStream stream(file);
		 stream << "Scorer";
		 for (int i = 0; i < m_keypointNameList.size()*3; i++) {
			 stream << "," << m_scorer;
		 }
		 stream << "\n";
		 stream << "entities";
		 for (int i = 0; i < m_keypointNameList.size()*3; i++) {
			 stream << "," << m_entityNameList[i/3];
		 }
		 stream << "\n";
		 stream << "bodyparts";
		 for (int i = 0; i < m_keypointNameList.size()*3; i++) {
			 stream << "," << m_keypointNameList[i/3];
		 }
		 stream << "\n";
		 stream << "coords";
		 for (int i = 0; i < m_keypointNameList.size()*3; i++) {
			 if (i%3 == 1) stream << "," << "x";
			 else if (i%3 == 2) stream << "," << "state";
			 else stream << "," << "y";
		 }
		 stream << "\n";
	}
	for (auto& imgSet : m_imgSets) {
		for (int cam = 0; cam < m_numCameras; cam++) {
			QTextStream stream(saveFiles[cam]);
			stream << imgSet->frames[cam]->imagePath.split("/").last() << ",";	//decide on what should be in this path (look at DLC2.2)
			for (int i = 0; i < m_keypointNameList.size(); i++) {
				Keypoint *keypoint =  imgSet->frames[cam]->keypointMap[m_entityNameList[i] + "/" + m_keypointNameList[i]];
				if (keypoint->state() == NotAnnotated) {
					stream << ",," << 0 << ",";
				}
				else if (keypoint->state() == Annotated) {
					stream << keypoint->rx() << "," << keypoint->ry() << ",";
					stream << 1 << ",";
				}
				else if (keypoint->state() == Reprojected) {
					stream << keypoint->rx() << "," << keypoint->ry() << ",";
					stream << 2 << ",";
				}
				else {
					stream << ",," << 3 << ",";
				}
			}
			stream << "\n";
		}
	}
	for (int i = 0; i < m_numCameras; i++) {
		saveFiles[i]->close();
	}
}


bool Dataset::GetImageSizeEx(QString fn, int *x,int *y) {
    FILE *f=fopen(fn.toStdString().c_str(),"rb");
    if (f==0) return false;
    fseek(f,0,SEEK_END);
    long len=ftell(f);
    fseek(f,0,SEEK_SET);
    if (len<24) {
        fclose(f);
        return false;
    }
    unsigned char buf[24]; fread(buf,1,24,f);

    if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0 && buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F') {
        long pos=2;
        while (buf[2]==0xFF) {
            if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3 || buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB)
                break;
            pos += 2+(buf[4]<<8)+buf[5];
            if (pos+12>len) break;
            fseek(f,pos,SEEK_SET);
            fread(buf+2,1,12,f);
        }
    }
    fclose(f);

  // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
    if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF) {
        *y = (buf[7]<<8) + buf[8];
        *x = (buf[9]<<8) + buf[10];
        return true;
    }

  // GIF: first three bytes say "GIF", next three give version number. Then dimensions
    if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F') {
        *x = buf[6] + (buf[7]<<8);
        *y = buf[8] + (buf[9]<<8);
        return true;
    }

  // PNG: the first frame is by definition an IHDR frame, which gives dimensions
    if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R') {
        *x = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
        *y = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
        return true;
    }

    return false;
}
