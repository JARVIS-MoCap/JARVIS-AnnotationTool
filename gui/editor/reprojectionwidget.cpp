/*****************************************************************
	* File:			  reprojectionwidget.cpp
	* Created: 	  03. December 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "reprojectionwidget.hpp"

#include <QFileDialog>


ReprojectionWidget::ReprojectionWidget(QWidget *parent) : QWidget(parent) {
	settings = new QSettings();
	m_colorMap = new ColorMap(ColorMap::Jet);
	QGridLayout *reprojectionlayout = new QGridLayout(this);

	QLabel *reprojectionLabel = new QLabel("Reprojection Tool");
	reprojectionLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	toggleSwitch = new Switch(this);
	toggleSwitch->setMinimumSize(55,25);
	toggleSwitch->setMaximumSize(55,25);
	toggleSwitch->setEnabled(false);
	connect(toggleSwitch, &Switch::toggled, this, &ReprojectionWidget::switchToggledSlot);

	modeLabel = new QLabel("Mode");
	modeLabel->hide();
	modeCombo = new QComboBox(this);
	modeCombo->addItem("Reprojection");
	modeCombo->addItem("Bone Length");
	connect(modeCombo, &QComboBox::currentTextChanged, this, &ReprojectionWidget::modeComboChangedSlot);
	modeCombo->hide();

	stackedWidget = new QStackedWidget(this);

	QLabel *loadDatasetFirstLabel = new QLabel("Load Datset First...");
	loadDatasetFirstLabel->setFont(QFont("Sans Serif", 14, QFont::Bold));
	loadDatasetFirstLabel->setAlignment(Qt::AlignCenter);
	stackedWidget->addWidget(loadDatasetFirstLabel);

	calibrationSetup = new QWidget(stackedWidget);
	QGridLayout *calibrationsetuplayout = new QGridLayout(calibrationSetup);

	initReprojectionButton = new QPushButton();
	initReprojectionButton->setIcon(QIcon::fromTheme("add"));
	initReprojectionButton->setIconSize(QSize(100,100));
	initReprojectionButton->setStyleSheet("QPushButton {border-radius: 10px;"
				"background-color: palette(window);"
				"border: 4px solid rgb(32,100,164);}"
				"QPushButton:hover{ background-color: rgb(68,74,89);}");
	initReprojectionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(initReprojectionButton, &QPushButton::clicked, this, &ReprojectionWidget::initReprojectionClickedSlot);

	calibrationsetuplayout->addWidget(initReprojectionButton,0,0);

	reprojectionChartWidget = new ReprojectionChartWidget(this);
	boneLengthChartWidget = new BoneLengthChartWidget(this);

	stackedWidget->addWidget(loadDatasetFirstLabel);
	stackedWidget->addWidget(calibrationSetup);
	stackedWidget->addWidget(reprojectionChartWidget);
	stackedWidget->addWidget(boneLengthChartWidget);

	reprojectionlayout->addWidget(reprojectionLabel,0,0);
	reprojectionlayout->addWidget(toggleSwitch,0,1, Qt::AlignRight);
	reprojectionlayout->addWidget(modeLabel,1,0);
	reprojectionlayout->addWidget(modeCombo,1,1);
	reprojectionlayout->addWidget(stackedWidget,2,0,1,2);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals
	connect(this, &ReprojectionWidget::datasetLoaded, reprojectionChartWidget, &ReprojectionChartWidget::datasetLoadedSlot);
	connect(this, &ReprojectionWidget::datasetLoaded, boneLengthChartWidget, &BoneLengthChartWidget::datasetLoadedSlot);
	connect(this, &ReprojectionWidget::errorThresholdChanged, reprojectionChartWidget, &ReprojectionChartWidget::errorThresholdChanged);
	connect(this, &ReprojectionWidget::boneLengthErrorThresholdChanged, boneLengthChartWidget, &BoneLengthChartWidget::boneLengthErrorThresholdChanged);
	//<-> Relayed Signals
}


void ReprojectionWidget::datasetLoadedSlot() {
	m_currentImgSetIndex = 0;
	m_currentFrameIndex = 0;
	m_numCameras = Dataset::dataset->numCameras();
	m_entitiesList = Dataset::dataset->entitiesList();
	m_bodypartsList = Dataset::dataset->bodypartsList();

	stackedWidget->setCurrentWidget(calibrationSetup);
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		m_reprojectionErrors[entity] = new std::vector<double>(Dataset::dataset->bodypartsList().size());
		m_boneLengthErrors[entity] = new std::vector<double>(Dataset::dataset->skeleton().size());
	}

	switchToggledSlot(false);
	QDir dir(Dataset::dataset->datasetBaseFolder() + "/CalibrationParameters");
	emit datasetLoaded();
	if (dir.exists()) {
		m_calibExists = true;
		initReprojectionClickedSlot();
	}
	else {
		m_calibExists = false;
	}
}


void ReprojectionWidget::switchToggledSlot(bool toggle) {
	if (toggle) {
		if (Dataset::dataset == nullptr) {
			toggleSwitch->setToggled(false);
			return;
		}
		m_reprojectionActive = true;
		calculateAllReprojections();
		calculateReprojectionSlot(m_currentImgSetIndex, m_currentFrameIndex);
	}
	else {
		m_reprojectionActive = false;
		undoReprojection();
	}
	emit reprojectedPoints(Dataset::dataset->imgSets()[m_currentImgSetIndex], m_currentFrameIndex);
	emit reprojectionToolToggled(toggle);
}


bool ReprojectionWidget::checkCalibParams(const QString &path) {
	for (const auto & cameraName : Dataset::dataset->cameraNames()) {
		QFileInfo calibFile(path + "/" + cameraName + ".yaml");
		if (!calibFile.exists()) {
			return false;
		}
	}
	return true;
}

void ReprojectionWidget::initReprojectionClickedSlot() {
	QString path = Dataset::dataset->datasetBaseFolder() + "/CalibrationParameters";
	if (!m_calibExists) {
		QString newCalibPath = QFileDialog::getExistingDirectory(this, "Select CalibrationParameters Path", m_parameterDir.path());
		if (path == "" || !checkCalibParams(newCalibPath)) return;
		QDir dir;
		dir.mkpath(path);
		QDir calibDir(newCalibPath);
		QStringList calibFiles = calibDir.entryList(QStringList() << "*.yaml",QDir::Files);
		foreach(QString filename, calibFiles) {
		QFile calibFile(newCalibPath + "/" + filename);
		calibFile.copy(path + "/" + filename);
		}
		path = newCalibPath;
	}
	else {
		if (!checkCalibParams(path)) {
			qCritical() << "Wrong Calibration Files!";
			m_calibExists = false;
			return;
		}
	}
	getSettings();
	toggleSwitch->setEnabled(true);
	toggleSwitch->setToggled(true);
	emit reprojectionToolToggled(true);
	m_reprojectionActive = true;
	QList<QString> intrinsicsList;
	for (int cam = 0; cam < m_numCameras; cam++) {
		intrinsicsList.append(path + "/" + Dataset::dataset->cameraName(cam) + ".yaml");
	}
	QList<QString> extrinsicsList;
	reprojectionTool = new ReprojectionTool(intrinsicsList, extrinsicsList,0);
	stackedWidget->setCurrentWidget(reprojectionChartWidget);
	modeLabel->show();
	modeCombo->show();
	calculateAllReprojections();
	calculateReprojectionSlot(m_currentImgSetIndex, m_currentFrameIndex);
	emit reprojectionToolUpdated(reprojectionTool);
}


void ReprojectionWidget::calculateReprojectionSlot(int currentImgSetIndex, int currentFrameIndex) {
	m_currentImgSetIndex = currentImgSetIndex;
	m_currentFrameIndex = currentFrameIndex;
	QMap<QString, cv::Mat> reconPointsMap;
	QMap<QString, QVector3D> coords3D;
	if (m_reprojectionActive) {
		for (const auto& entity : m_entitiesList) {
			for (const auto& bodypart : m_bodypartsList) {
				QList<int> camsToUse;
				QList<int> alreadyReprojected;
				QList<QPointF> points;
				int camCounter = 0;
				for (const auto& frame : Dataset::dataset->imgSets()[currentImgSetIndex]->frames) {
					if (frame->keypointMap[entity + "/" + bodypart]->state() == Annotated) {
						camsToUse.append(camCounter);
						points.append(frame->keypointMap[entity + "/" + bodypart]->coordinates());
					}
					else if (frame->keypointMap[entity + "/" + bodypart]->state() == Reprojected) {
						alreadyReprojected.append(camCounter);
					}
					camCounter++;
				}
				if (camsToUse.size() >= m_minViews) {
					cv::Mat X =  reprojectionTool->reconstructPoint3D(points, camsToUse);
					coords3D[entity + "/" + bodypart] = QVector3D(X.at<double>(0), X.at<double>(1), X.at<double>(2));
					reconPointsMap[entity + "/" + bodypart] = X;
					QList<QPointF> reprojectedPoints = reprojectionTool->reprojectPoint(X);
					double reprojectionError = 0;
					for (int cam = 0; cam < reprojectedPoints.size(); cam ++) {
						QSize imgSize = Dataset::dataset->imgSets()[currentImgSetIndex]->frames[cam]->imageDimensions;
						QRectF imgRect(QPoint(0,0), imgSize);
						Keypoint *keypoint = Dataset::dataset->imgSets()[currentImgSetIndex]->frames[cam]->keypointMap[entity + "/" + bodypart];
						if (!camsToUse.contains(cam) && imgRect.contains(reprojectedPoints[cam])) {
							if (keypoint->state() != Suppressed) {
								keypoint->setState(Reprojected);
								keypoint->setCoordinates(reprojectedPoints[cam]);
							}
						}
						else if (keypoint->state() == Annotated) {
							QPointF annotatedPoint = Dataset::dataset->imgSets()[currentImgSetIndex]->frames[cam]->keypointMap[entity + "/" + bodypart]->coordinates();
							QPointF dist = annotatedPoint-reprojectedPoints[cam];
							reprojectionError += sqrt(dist.x()*dist.x()+dist.y()*dist.y())/reprojectedPoints.size();
						}
					}
						(*m_reprojectionErrors[entity])[Dataset::dataset->bodypartsList().indexOf(bodypart)] = reprojectionError;
				}
				else {
					(*m_reprojectionErrors[entity])[Dataset::dataset->bodypartsList().indexOf(bodypart)] = 0;
					for (int cam = 0; cam < Dataset::dataset->numCameras(); cam ++) {
						Keypoint *keypoint = Dataset::dataset->imgSets()[currentImgSetIndex]->frames[cam]->keypointMap[entity + "/" + bodypart];
						if (keypoint->state() == Reprojected) {
							keypoint->setState(NotAnnotated);
						}
					}
				}
			}
		}
		for (const auto& entity : m_entitiesList) {
			int idx = 0;
			for (const auto& comp : Dataset::dataset->skeleton()) {
				if (reconPointsMap.contains(entity + "/" + comp.keypointA) && reconPointsMap.contains(entity + "/" + comp.keypointB)) {
					double dist = cv::norm(reconPointsMap[entity + "/" + comp.keypointA], reconPointsMap[entity + "/" +  comp.keypointB]);
					(*m_boneLengthErrors[entity])[idx++] = dist - comp.length;
				}
				else {
					(*m_boneLengthErrors[entity])[idx++] = 0.0;
				}
			}
		}
		emit reprojectedPoints(Dataset::dataset->imgSets()[currentImgSetIndex], currentFrameIndex);
		emit update3DCoords(coords3D);

		emit reprojectionToolToggled(true);
		reprojectionChartWidget->reprojectionErrorsUpdatedSlot(m_reprojectionErrors);
		boneLengthChartWidget->boneLengthErrorsUpdatedSlot(m_boneLengthErrors);
	}
}


void ReprojectionWidget::calculateAllReprojections() {
	if (m_reprojectionActive) {
		for (const auto& imgSet : Dataset::dataset->imgSets()) {
			for (const auto& entity : m_entitiesList) {
				for (const auto& bodypart : m_bodypartsList) {
					QList<int> camsToUse;
					QList<int> alreadyReprojected;
					QList<QPointF> points;
					int camCounter = 0;
					for (const auto& frame : imgSet->frames) {
						if (frame->keypointMap[entity + "/" + bodypart]->state() == Annotated) {
							camsToUse.append(camCounter);
							points.append(frame->keypointMap[entity + "/" + bodypart]->coordinates());
						}
						else if (frame->keypointMap[entity + "/" + bodypart]->state() == Reprojected) {
							alreadyReprojected.append(camCounter);
						}
						camCounter++;
					}
					if (camsToUse.size() >= m_minViews) {
						cv::Mat X =  reprojectionTool->reconstructPoint3D(points, camsToUse);
						QList<QPointF> reprojectedPoints = reprojectionTool->reprojectPoint(X);
						double reprojectionError = 0;
						for (int cam = 0; cam < reprojectedPoints.size(); cam ++) {
							QSize imgSize =imgSet->frames[cam]->imageDimensions;
							QRectF imgRect(QPoint(0,0), imgSize);
							Keypoint *keypoint = imgSet->frames[cam]->keypointMap[entity + "/" + bodypart];
							if (!camsToUse.contains(cam) && imgRect.contains(reprojectedPoints[cam])) {
								if (keypoint->state() != Suppressed) {
									keypoint->setState(Reprojected);
									keypoint->setCoordinates(reprojectedPoints[cam]);
								}
							}
							else if (keypoint->state() == Annotated) {
								QPointF annotatedPoint = imgSet->frames[cam]->keypointMap[entity + "/" + bodypart]->coordinates();
								QPointF dist = annotatedPoint-reprojectedPoints[cam];
								reprojectionError += sqrt(dist.x()*dist.x()+dist.y()*dist.y())/reprojectedPoints.size();
							}
						}
							(*m_reprojectionErrors[entity])[Dataset::dataset->bodypartsList().indexOf(bodypart)] = reprojectionError;
					}
					else {
						(*m_reprojectionErrors[entity])[Dataset::dataset->bodypartsList().indexOf(bodypart)] = 0;
						for (int cam = 0; cam < Dataset::dataset->numCameras(); cam ++) {
							Keypoint *keypoint = imgSet->frames[cam]->keypointMap[entity + "/" + bodypart];
							if (keypoint->state() == Reprojected) {
								keypoint->setState(NotAnnotated);
							}
						}
					}
				}
			}
		}
	}
}


void ReprojectionWidget::undoReprojection() {
	for (auto& imgSet : Dataset::dataset->imgSets()) {
		for (auto& frame : imgSet->frames) {
			for (auto& keypoint : frame->keypoints) {
				if (keypoint->state() == Reprojected) keypoint->setState(NotAnnotated);
			}
		}
	}
}


void ReprojectionWidget::getSettings() {
	settings->beginGroup("Settings");
	settings->beginGroup("ReprojectionSettings");
	if (settings->contains("MinViews")) {
		m_minViews = settings->value("MinViews").toInt();
	}
	minViewsChangedSlot(m_minViews);
	// if (settings->contains("errorThreshold")) {
	// 	m_errorThreshold = settings->value("errorThreshold").toDouble();
	// }
	// errorThresholdChangedSlot(m_errorThreshold);
	settings->endGroup();
	settings->endGroup();
}


void ReprojectionWidget::minViewsChangedSlot(int value) {
	m_minViews = value;
	calculateAllReprojections();
	calculateReprojectionSlot(m_currentImgSetIndex, m_currentFrameIndex);
}


// void ReprojectionWidget::errorThresholdChangedSlot(double value) {
// 	m_errorThreshold = value;
// 	emit errorThresholdChanged(value);
// }

void ReprojectionWidget::modeComboChangedSlot(const QString& mode) {
	if (mode == "Reprojection") {
		stackedWidget->setCurrentWidget(reprojectionChartWidget);
	}
	else {
		stackedWidget->setCurrentWidget(boneLengthChartWidget);

	}
}
