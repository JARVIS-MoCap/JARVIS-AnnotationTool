/*******************************************************************************
 * File:		  visualizationwindow.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "visualizationwindow.hpp"
#include "dataset.hpp"

#include <QSpinBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QTimer>


//using namespace QtDataVisualization;

VisualizationWindow::VisualizationWindow(QMainWindow *parent) : QMainWindow(parent)
{
	this->resize(600,600);
	this->setMinimumSize(500,550);
	settings = new QSettings();
	setWindowTitle("Visualization");
	QWidget *mainWidget = new QWidget();
	this->setCentralWidget(mainWidget);
	QGridLayout *layout = new QGridLayout(mainWidget);

	viewPort = new ViewPort(mainWidget);
	viewPort->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(viewPort, &ViewPort::cameraMoved, this, &VisualizationWindow::cameraMovedSlot);

	visToolBar = new VisualizationToolBar(this);
	this->addToolBar(visToolBar);
	connect(visToolBar, &VisualizationToolBar::toggleCameras, viewPort, &ViewPort::toggleCamerasSlot);
	connect(visToolBar, &VisualizationToolBar::toggleSetup, viewPort, &ViewPort::toggleSetupSlot);
	connect(visToolBar, &VisualizationToolBar::toggleViewAdjust, viewPort, &ViewPort::toggleViewAdjustSlot);
	connect(visToolBar, &VisualizationToolBar::saveClicked, this, &VisualizationWindow::saveClickedSlot);
	connect(visToolBar, &VisualizationToolBar::loadClicked, this, &VisualizationWindow::loadClickedSlot);

	controlDock = new ControlDock(this);
	this->addDockWidget(Qt::BottomDockWidgetArea,controlDock);
	connect(controlDock, &ControlDock::cameraViewChanged, this, &VisualizationWindow::cameraViewChangedSlot);
	connect(controlDock, &ControlDock::keypointRadiusChanged, viewPort, &ViewPort::setKeypointRadiusSlot);
	connect(controlDock, &ControlDock::skeletonThicknessChanged, viewPort, &ViewPort::setSkeletonThicknessSlot);


	layout->addWidget(viewPort, 0,0);
}

void VisualizationWindow::opened() {
	QTimer::singleShot(200, this, SLOT(viewInitSlot()));
}

void VisualizationWindow::viewInitSlot() {
	viewPort->toggleCamerasSlot(false);
	cameraViewChangedSlot(0);
}

void VisualizationWindow::update3DCoordsSlot(QMap<QString, QVector3D> coords3D)
{
	viewPort->setCoords3D(coords3D);
	viewPort->update();
}

void VisualizationWindow::reprojectionToolUpdatedSlot(ReprojectionTool *reproTool) 
{
	viewPort->reset();
	m_reproTool = reproTool;
	auto cameraNames = m_reproTool->cameraNames();
	controlDock->updateCameraButtons(cameraNames);
	drawCameras();
	QSize imgSize = Dataset::dataset->imgSets()[0]->frames[0]->imageDimensions;
	float focalLength = m_reproTool->intrinsicsList()[0].intrinsicMatrix.at<double>(0,0);
	viewPort->setCameraLense(focalLength, imgSize);
}

void VisualizationWindow::cameraViewChangedSlot(int index) {
	ReprojectionTool::CameraExtrinsics extrinsics = m_reproTool->extrinsicsList()[index];
	m_camMoveCounter = 0;

	cv::Mat translationVector = - extrinsics.rotationMatrix * extrinsics.translationVector;
	QVector3D position(translationVector.at<double>(0), translationVector.at<double>(1), translationVector.at<double>(2));

	cv::Mat viewCenter = cv::Mat::zeros(cv::Size(1,3), CV_64F);
	viewCenter.at<double>(0) = 0; viewCenter.at<double>(1) = 0; viewCenter.at<double>(2) = 1;
	viewCenter = extrinsics.rotationMatrix * viewCenter;
	QVector3D pointingVector(viewCenter.at<double>(0),viewCenter.at<double>(1),viewCenter.at<double>(2));

	cv::Mat upVect = cv::Mat::zeros(cv::Size(1,3), CV_64F);
	upVect.at<double>(0) = 0; upVect.at<double>(1) = -1; upVect.at<double>(2) = 0;
	upVect = extrinsics.rotationMatrix * upVect;

	QVector3D upVector(upVect.at<double>(0),upVect.at<double>(1),upVect.at<double>(2));
	viewPort->setCameraPosition(position, upVector, viewPort->center());
}

void VisualizationWindow::drawCameras() {
	int idx = 0;
	for (const auto &extrinsics :  m_reproTool->extrinsicsList()) {
		cv::Mat translationVector = - extrinsics.rotationMatrix * extrinsics.translationVector;
		QVector3D position(translationVector.at<double>(0), translationVector.at<double>(1), translationVector.at<double>(2));

		cv::Mat upVect = cv::Mat::zeros(cv::Size(1,3), CV_64F);
		upVect.at<double>(0) = 0; upVect.at<double>(1) = -1; upVect.at<double>(2) = 0;
		upVect = extrinsics.rotationMatrix * upVect;
		QVector3D upVector(upVect.at<double>(0),upVect.at<double>(1),upVect.at<double>(2));

		cv::Mat viewCenter = cv::Mat::zeros(cv::Size(1,3), CV_64F);
		viewCenter.at<double>(0) = 0; viewCenter.at<double>(1) = 0; viewCenter.at<double>(2) = 1;
		viewCenter = extrinsics.rotationMatrix * viewCenter;
		QVector3D pointingVector(viewCenter.at<double>(0),viewCenter.at<double>(1),viewCenter.at<double>(2));
		viewPort->addCamera(position, pointingVector, upVector);
	}
}

void VisualizationWindow::cameraMovedSlot() {
	m_camMoveCounter++;
	if (m_camMoveCounter == 9) {
		controlDock->resetCameraButtons();
	}
}

void VisualizationWindow::saveClickedSlot() {
	QFileDialog fd(this);
	QString fileName = fd.getSaveFileName(this, "Save Keypoints to File",
											Dataset::dataset->datasetFolder(), tr("CSV Files (*.csv)"));
	fd.setAcceptMode(QFileDialog::AcceptSave);
	QFile saveFile(fileName);
	if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&saveFile);
		out << "Entity" << ","<< "Keypoint" << "," << "x" << "," << "y" << "," << "z" << "\n";
		auto coords3D = viewPort->coords3D();
		for (const auto & id : coords3D.keys()) {
			float x = coords3D[id][0];
			float y = coords3D[id][1];
			float z = coords3D[id][2];
			out << id.split("/")[0] << "," << id.split("/")[1] << "," << x << "," << y << "," << z << "\n";
		}
	}
	else {
		qCritical() << "Could not save Keypoints to file!";
	}
	saveFile.close();
}

void VisualizationWindow::loadClickedSlot() {
	QFileDialog fd(this);
	QString fileName = fd.getOpenFileName(this, "Load Setup Keypoint Savefile",
				Dataset::dataset->datasetFolder(), tr("CSV Files (*.csv)"));
	fd.setFileMode(	QFileDialog::ExistingFile);
	QFile saveFile(fileName);
	QList<QStringList> fileText;
	QList<QVector3D> coords;
	if (saveFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream s1(&saveFile);
		while (!s1.atEnd()) {
		  QString s=s1.readLine();
		  fileText.append(s.split(","));
		}
		saveFile.close();
		for (int row = 1; row < fileText.size(); row++) {
			if (fileText[row].size() != 5) {
				qCritical() << "Could not load Keypoints! Wrong format!";
				return;
			}
			QString name = fileText[row][0] + "/" + fileText[row][1];
			QVector3D coord(fileText[row][2].toFloat(), fileText[row][3].toFloat(), fileText[row][4].toFloat());
			coords.append(coord);
		}
	}

	if (coords.size() == 0) return;
	viewPort->addSetup(coords, 10, QColor(100,164,32), QColor(32,100,164), 0.4, true);

}
