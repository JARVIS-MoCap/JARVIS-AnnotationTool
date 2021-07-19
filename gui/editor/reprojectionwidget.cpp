/*------------------------------------------------------------
 *  reprojectionwidget.cpp
 *  Created:  03. December 2020
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "reprojectionwidget.hpp"

#include <QFileDialog>

ReprojectionWidget::ReprojectionWidget(QWidget *parent) : QWidget(parent) {
	settings = new QSettings();
	m_colorMap = new ColorMap(ColorMap::Jet);
	QGridLayout *reprojectionlayout = new QGridLayout(this);

	QLabel *reprojectionLabel = new QLabel("Reprojection Tool");
	reprojectionLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	toggleSwitch = new Switch(this);
	toggleSwitch->setMaximumSize(55,25);
	toggleSwitch->setEnabled(false);
	connect(toggleSwitch, &Switch::toggled, this, &ReprojectionWidget::switchToggledSlot);

	stackedWidget = new QStackedWidget(this);

	QLabel *loadDatasetFirstLabel = new QLabel("Load Datset First...");
	loadDatasetFirstLabel->setFont(QFont("Sans Serif", 14, QFont::Bold));
	loadDatasetFirstLabel->setAlignment(Qt::AlignCenter);
	stackedWidget->addWidget(loadDatasetFirstLabel);

	intrinsicsSetup = new QWidget(stackedWidget);
	intrinsicssetuplayout = new QGridLayout(intrinsicsSetup);
	QGroupBox *intrinsicsInfoBox = new QGroupBox(intrinsicsSetup);
	QGridLayout *intrinsicsinfolayout = new QGridLayout(intrinsicsInfoBox);
	intrinsicsinfolayout->setMargin(4);
	intrinsicsinfolayout->setSpacing(10);
	QLabel *intrinsicsInfoIcon = new QLabel();
	intrinsicsInfoIcon->setPixmap(QIcon::fromTheme("info2").pixmap(QSize(60, 60)));
	QLabel *intrinsicsInfoLabel = new QLabel("Select Savefiles containing the intrinsic parameters for all the cameras obtained by calibration.");
	intrinsicsInfoLabel->setWordWrap(true);
	intrinsicsinfolayout->addWidget(intrinsicsInfoIcon,0,0);
	intrinsicsinfolayout->addWidget(intrinsicsInfoLabel,0,1);
	intrinsicsBox = new QGroupBox("Intrinsic Camera Parameters");
	intrinsicslayout = new QGridLayout(intrinsicsBox);
	initIntrinsicsButton = new QPushButton("Set Intrinsic Parameters");
	initIntrinsicsButton->setEnabled(false);
	connect(initIntrinsicsButton, &QPushButton::clicked, this, &ReprojectionWidget::initIntrinsicsClickedSlot);
	intrinsicssetuplayout->addWidget(intrinsicsInfoBox,0,0);
	intrinsicssetuplayout->addWidget(intrinsicsBox,1,0);
	intrinsicssetuplayout->addWidget(initIntrinsicsButton,2,0,Qt::AlignRight);

	extrinsicsSetup = new QWidget(stackedWidget);
	extrinsicssetuplayout = new QGridLayout(extrinsicsSetup);
	QGroupBox *extrinsicsInfoBox = new QGroupBox(intrinsicsSetup);
	QGridLayout *extrinsicsinfolayout = new QGridLayout(extrinsicsInfoBox);
	extrinsicsinfolayout->setMargin(4);
	extrinsicsinfolayout->setSpacing(10);
	QLabel *extrinsicsInfoIcon = new QLabel();
	extrinsicsInfoIcon->setPixmap(QIcon::fromTheme("info2").pixmap(QSize(60, 60)));
	QLabel *extrinsicsInfoLabel = new QLabel("Select Savefiles containing the extrinsic parameters for all camera pairs obtained by stereo calibration.");
	extrinsicsInfoLabel->setWordWrap(true);
	extrinsicsinfolayout->addWidget(extrinsicsInfoIcon,0,0);
	extrinsicsinfolayout->addWidget(extrinsicsInfoLabel,0,1);
	extrinsicsBox = new QGroupBox("Extrinsic Camera Parameters");
	extrinsicslayout = new QGridLayout(extrinsicsBox);
	QWidget *primarySelectorWidget = new QWidget(this);
	QGridLayout *primaryselectorlayout = new QGridLayout(primarySelectorWidget);
	primaryselectorlayout->setMargin(0);
	QLabel *primaryLabel = new QLabel("Primary Camera");
	primaryCombo = new QComboBox(extrinsicsBox);
	primaryselectorlayout->addWidget(primaryLabel,0,0);
	primaryselectorlayout->addWidget(primaryCombo,0,1);
	extrinsicslayout->addWidget(primarySelectorWidget,0,0,1,3);
	initReprojectionButton = new QPushButton("Initialise Reprojection Tool");
	initReprojectionButton->setEnabled(false);
	connect(initReprojectionButton, &QPushButton::clicked, this, &ReprojectionWidget::initReprojectionClickedSlot);
	extrinsicssetuplayout->addWidget(extrinsicsInfoBox,0,0);
	extrinsicssetuplayout->addWidget(extrinsicsBox,1,0);
	extrinsicssetuplayout->addWidget(initReprojectionButton,2,0, Qt::AlignRight);

	reprojectionController = new QWidget(stackedWidget);
	reprojectioncontrollerlayout = new QGridLayout(reprojectionController);
	reprojectionSettingsBox = new QGroupBox("Settings");
	reprojectionSettingsBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QGridLayout* reprojectionsettingslayout = new QGridLayout(reprojectionSettingsBox);
	QLabel *minViewsLabel = new QLabel("Min Annotated Views");
	minViewsEdit = new QSpinBox();
	minViewsEdit->setValue(2);
	connect(minViewsEdit, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ReprojectionWidget::minViewsChangedSlot);
	QLabel *errorThresholdLabel = new QLabel("Repro Error Threshold");
	errorThresholdEdit = new QDoubleSpinBox();
	errorThresholdEdit->setValue(10.0);
	connect(errorThresholdEdit, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ReprojectionWidget::errorThresholdChangedSlot);
	reprojectionChartWidget = new ReprojectionChartWidget(this);

	reprojectionsettingslayout->addWidget(minViewsLabel,0,0);
	reprojectionsettingslayout->addWidget(minViewsEdit,0,1);
	reprojectionsettingslayout->addWidget(errorThresholdLabel,1,0);
	reprojectionsettingslayout->addWidget(errorThresholdEdit,1,1);

	reprojectioncontrollerlayout->addWidget(reprojectionSettingsBox,0,0);
	reprojectioncontrollerlayout->addWidget(reprojectionChartWidget,1,0);

	stackedWidget->addWidget(intrinsicsSetup);
	stackedWidget->addWidget(extrinsicsSetup);
	stackedWidget->addWidget(reprojectionController);

	reprojectionlayout->addWidget(reprojectionLabel,0,0);
	reprojectionlayout->addWidget(toggleSwitch,0,1);
	reprojectionlayout->addWidget(stackedWidget,1,0,1,2);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals
	connect(this, &ReprojectionWidget::datasetLoaded, reprojectionChartWidget, &ReprojectionChartWidget::datasetLoadedSlot);
	connect(this, &ReprojectionWidget::errorThresholdChanged, reprojectionChartWidget, &ReprojectionChartWidget::errorThresholdChanged);
	//<-> Relayed Signals
}


void ReprojectionWidget::datasetLoadedSlot() {
	m_numCameras = Dataset::dataset->numCameras();
	m_entitiesList = Dataset::dataset->entitiesList();
	m_bodypartsList = Dataset::dataset->bodypartsList();
	for (int i = 0; i < m_numCameras; i++) {
		QLabel *camLabel = new QLabel(Dataset::dataset->cameraName(i) ,intrinsicsBox);
		QLineEdit *pathEdit = new QLineEdit(intrinsicsBox);
		intrinsicsPathEdits.append(pathEdit);
		QPushButton *pathButton = new QPushButton();
		pathButton->setMinimumSize(30,30);
		pathButton->setIcon(QIcon::fromTheme("folder"));
		connect(pathButton, &QPushButton::clicked, this, &ReprojectionWidget::intrinsicsPathClickedSlot);
		intrinsicslayout->addWidget(camLabel,i,0);
		intrinsicslayout->addWidget(pathEdit,i,1);
		intrinsicslayout->addWidget(pathButton,i,2);
	}
	for (int cam = 0; cam < m_numCameras; cam++) {
		primaryCombo->addItem(Dataset::dataset->cameraName(cam));
	}
	connect(primaryCombo, &QComboBox::currentTextChanged, this, &ReprojectionWidget::primaryChangedSlot);
	for (int cam = 0; cam < m_numCameras; cam++) {
		QLabel *camLabel = new QLabel(Dataset::dataset->cameraName(cam) ,intrinsicsBox);
		QLineEdit *pathEdit = new QLineEdit(intrinsicsBox);
		extrinsicsPathEdits.append(pathEdit);
		QPushButton *pathButton = new QPushButton();
		pathButton->setMinimumSize(30,30);
		pathButton->setIcon(QIcon::fromTheme("folder"));
		connect(pathButton, &QPushButton::clicked, this, &ReprojectionWidget::extrinsicsPathClickedSlot);
		extrinsicslayout->addWidget(camLabel,cam+1,0);
		extrinsicslayout->addWidget(pathEdit,cam+1,1);
		extrinsicslayout->addWidget(pathButton,cam+1,2);
		if (cam == primaryCombo->currentIndex()) {
			camLabel->hide();
			pathEdit->hide();
			pathButton->hide();
		}
	}
	minViewsEdit->setRange(2, m_numCameras);
	loadPaths();
	stackedWidget->setCurrentWidget(intrinsicsSetup);
	checkIntrinsicPathsAdded();
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		m_reprojectionErrors[entity] = new std::vector<double>(Dataset::dataset->bodypartsList().size());
	}
	emit datasetLoaded();
}


void ReprojectionWidget::switchToggledSlot(bool toggle) {
	if (toggle) {
		if (Dataset::dataset == nullptr) {
			toggleSwitch->setToggled(false);
			return;
		}
		m_reprojectionActive = true;
		calculateReprojectionSlot(m_currentImgSetIndex, m_currentFrameIndex);
	}
	else {
		m_reprojectionActive = false;
		undoReprojection();
	}
	emit reprojectedPoints(Dataset::dataset->imgSets()[m_currentImgSetIndex], m_currentFrameIndex);
	emit reprojectionToolToggled(toggle);
}


void ReprojectionWidget::primaryChangedSlot(QString) {
	m_primaryIndex = primaryCombo->currentIndex();
	for (int cam = 0; cam < m_numCameras; cam++) {
		if (cam == primaryCombo->currentIndex()) {
			extrinsicslayout->itemAtPosition(cam+1,0)->widget()->hide();
			extrinsicslayout->itemAtPosition(cam+1,1)->widget()->hide();
			extrinsicslayout->itemAtPosition(cam+1,2)->widget()->hide();
		}
		else {
			extrinsicslayout->itemAtPosition(cam+1,0)->widget()->show();
			extrinsicslayout->itemAtPosition(cam+1,1)->widget()->show();
			extrinsicslayout->itemAtPosition(cam+1,2)->widget()->show();
		}
	}
	loadPaths(true);
	checkExtrinsicPathsAdded();
}

void ReprojectionWidget::intrinsicsPathClickedSlot() {
	int index = intrinsicslayout->indexOf(qobject_cast<QWidget*>(sender()));
	int row,_;
  intrinsicslayout->getItemPosition(index, &row, &_, &_, &_);
	QString default_path = ".";
	QString filename = QFileDialog::getOpenFileName(this, "Open Intrinsics of Camera " + QString::number(row+1), m_parameterDir.path(), "(*.yaml)");
	if (filename != "") {
		m_parameterDir.setPath(filename);
		m_parameterDir.cdUp();
		intrinsicsPathEdits[row]->setText(filename);
	}
	checkIntrinsicPathsAdded();
}

void ReprojectionWidget::extrinsicsPathClickedSlot() {
	int index = extrinsicslayout->indexOf(qobject_cast<QWidget*>(sender()));
	int row,_;
  extrinsicslayout->getItemPosition(index, &row, &_, &_, &_);
	QString filename = QFileDialog::getOpenFileName(this, "Open Intrinsics of Camera " + QString::number(row+1), m_parameterDir.path(), "(*.yaml)");
	if (filename != "") {
		m_parameterDir.setPath(filename);
		extrinsicsPathEdits[row-1]->setText(filename);
	}
	checkExtrinsicPathsAdded();
}

void ReprojectionWidget::checkIntrinsicPathsAdded() {
	bool allAdded = true;
	for (const auto& edit : intrinsicsPathEdits) {
		if (edit->text() == "" && edit->isVisible()) {
			allAdded = false;
			break;
		}
	}
	if (allAdded == true) {
		initIntrinsicsButton->setEnabled(true);
	}
}

void ReprojectionWidget::checkExtrinsicPathsAdded() {
	bool allAdded = true;
	for (const auto& edit : extrinsicsPathEdits) {
		if (edit->text() == "" && edit->isVisible()) {
			allAdded = false;
			break;
		}
	}
	if (allAdded == true) {
		initReprojectionButton->setEnabled(true);
	}
}

void ReprojectionWidget::initIntrinsicsClickedSlot() {
	for (const auto& edit : intrinsicsPathEdits) {
		if (!QFile::exists(edit->text())) {
			QErrorMessage *msg = new QErrorMessage();
			msg->showMessage("Intrinsics File '" + edit->text() + "' does not exist.");
			return;
		}
	}
	stackedWidget->setCurrentWidget(extrinsicsSetup);
	checkExtrinsicPathsAdded();
}

void ReprojectionWidget::initReprojectionClickedSlot() {
	for (const auto& edit : extrinsicsPathEdits) {
		if (edit->isVisible()) {
			if (!QFile::exists(edit->text())) {
				QErrorMessage *msg = new QErrorMessage();
				msg->showMessage("Extrinsics File '" + edit->text() + "' does not exist.");
				return;
			}
		}
	}
	savePaths();
	toggleSwitch->setEnabled(true);
	toggleSwitch->setToggled(true);
	emit reprojectionToolToggled(true);
	m_reprojectionActive = true;
	QList<QString> intrinsicsList;
	for (const auto& edit : intrinsicsPathEdits) {
		intrinsicsList.append(edit->text());
	}
	QList<QString> extrinsicsList;
	for (const auto& edit : extrinsicsPathEdits) {
		extrinsicsList.append(edit->text());
	}
	reprojectionTool = new ReprojectionTool(intrinsicsList, extrinsicsList,primaryCombo->currentIndex());
	stackedWidget->setCurrentWidget(reprojectionController);
	calculateReprojectionSlot(m_currentImgSetIndex, m_currentFrameIndex);
}

void ReprojectionWidget::savePaths() {
	settings->beginGroup("ReprojectionParameters");
	int i = 0;
	settings->beginGroup("Intrinsics");
	for (const auto& edit : intrinsicsPathEdits) {
		settings->setValue(Dataset::dataset->cameraName(i++), edit->text());
	}
	settings->endGroup();
	settings->beginGroup("Extrinsics");
	i = 0;
	for (const auto& edit : extrinsicsPathEdits) {
		settings->setValue(primaryCombo->currentText() + "/" + Dataset::dataset->cameraName(i++), edit->text());
	}
	settings->endGroup();
	settings->endGroup();
}

void ReprojectionWidget::loadPaths(bool onlyExtrinsics) {
	settings->beginGroup("ReprojectionParameters");
	int i = 0;
	if (!onlyExtrinsics) {
		settings->beginGroup("Intrinsics");
		if(settings->value(Dataset::dataset->cameraName(0)).toString() != "") {
			m_parameterDir.setPath(settings->value(Dataset::dataset->cameraName(0)).toString());
		}
		for (const auto& edit : intrinsicsPathEdits) {
			edit->setText(settings->value(Dataset::dataset->cameraName(i++)).toString());
		}
		settings->endGroup();
		i = 0;
	}
	settings->beginGroup("Extrinsics");
	if(settings->value(primaryCombo->currentText() + "/" + Dataset::dataset->cameraName(0)).toString() != "") {
		m_parameterDir.setPath(settings->value(primaryCombo->currentText() + "/" + Dataset::dataset->cameraName(0)).toString());
	}
	for (const auto& edit : extrinsicsPathEdits) {
		edit->setText(settings->value(primaryCombo->currentText() + "/" + Dataset::dataset->cameraName(i++)).toString());
	}
	settings->endGroup();
	settings->endGroup();
}

void ReprojectionWidget::calculateReprojectionSlot(int currentImgSetIndex, int currentFrameIndex) {
	m_currentImgSetIndex = currentImgSetIndex;
	m_currentFrameIndex = currentFrameIndex;
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
		emit reprojectedPoints(Dataset::dataset->imgSets()[currentImgSetIndex], currentFrameIndex);
		emit reprojectionToolToggled(true);
		reprojectionChartWidget->reprojectionErrorsUpdatedSlot(m_reprojectionErrors);
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

void ReprojectionWidget::minViewsChangedSlot(int value) {
	m_minViews = value;
	calculateReprojectionSlot(m_currentImgSetIndex, m_currentFrameIndex);
}

void ReprojectionWidget::errorThresholdChangedSlot(double value) {
	m_errorThreshold = value;
	emit errorThresholdChanged(value);
}
