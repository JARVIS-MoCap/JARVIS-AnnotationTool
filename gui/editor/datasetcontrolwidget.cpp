/*****************************************************************
	* File:			  datasetconrolwidget.cpp
	* Created: 	  03. December 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "datasetcontrolwidget.hpp"

#include <QHeaderView>


DatasetControlWidget::DatasetControlWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	QLabel *datasetControlLabel = new QLabel("Dataset Control");
	datasetControlLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));

	QLabel *segmentLabel = new QLabel("Segment");
	segmentCombo = new QComboBox(this);
	segmentCombo->setMinimumSize(200,25);
	connect(segmentCombo, &QComboBox::currentTextChanged, this, &DatasetControlWidget::segmentChangedSlot);

	QLabel *frameSetLabel = new QLabel("Frame Set");
	QWidget *frameSetWidget = new QWidget(this);
	QGridLayout *framesetwidgetlayout = new QGridLayout(frameSetWidget);
	framesetwidgetlayout->setMargin(0);
	frameSetEdit = new QLineEdit(frameSetWidget);
	frameSetEdit->setMinimumSize(40,25);
	frameSetEdit->setMaximumSize(50,25);
	frameSetEdit->setReadOnly(true);
	QLabel *seperatorLabel = new QLabel("/");
	QWidget *frameSetSpacer = new QWidget(frameSetWidget);
	frameSetSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	totalFrameSetLabel = new QLabel("");
	totalFrameSetLabel->setMinimumSize(40,25);
	totalFrameSetLabel->setMaximumSize(50,25);
	framesetwidgetlayout->addWidget(frameSetSpacer,0,0);
	framesetwidgetlayout->addWidget(frameSetEdit,0,1);
	framesetwidgetlayout->addWidget(seperatorLabel,0,2);
	framesetwidgetlayout->addWidget(totalFrameSetLabel,0,3);

	framesTable = new QTableWidget(0, 2);
	framesTable->setAlternatingRowColors(true);
	framesTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QStringList labels;
	labels << "Frame" << "Annotations";
	framesTable->setHorizontalHeaderLabels(labels);
	framesTable->setColumnWidth(1, 140);
	framesTable->horizontalHeader()-> setSectionResizeMode(0, QHeaderView::Stretch);
	framesTable->verticalHeader()->hide();
	framesTable->setShowGrid(false);
	framesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(framesTable, &QTableWidget::cellDoubleClicked, this, &DatasetControlWidget::selectionChangedSlot);

	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	layout->addWidget(datasetControlLabel,0,0,1,2);
	layout->addWidget(segmentLabel, 1,0);
	layout->addWidget(segmentCombo, 1,1);
	layout->addWidget(frameSetLabel,2,0);
	layout->addWidget(frameSetWidget,2,1);
	layout->addWidget(framesTable,3,0,1,2);
	layout->addWidget(spacer,4,0,1,2);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals

	//<-> Relayed Signals
}


void DatasetControlWidget::datasetLoadedSlot() {
	int currentIndex = 0;
	int segmentIndex;
	segmentCombo->blockSignals(true);
	segmentCombo->clear();
	segmentCombo->setEnabled(Dataset::dataset->segmentNames().length() != 1);
	for (const auto &segment : Dataset::dataset->segmentNames()) {
		segmentCombo->addItem(segment);
		if (segment.split("/").takeLast() == Dataset::dataset->datasetFolder().split("/").takeLast()) {
			segmentIndex = currentIndex;
			m_currentSegment = segment;
		}
		currentIndex++;
	}
	segmentCombo->setCurrentIndex(segmentIndex);
	segmentCombo->blockSignals(false);


	framesTable->setRowCount(Dataset::dataset->numCameras());
	m_annotatedCounts = new int [Dataset::dataset->numCameras()];
	int row = 0;
	for (const auto& cameraName : Dataset::dataset->cameraNames()) {
    QTableWidgetItem* frameItem = new QTableWidgetItem();
    frameItem->setText(cameraName);
		frameItem->setFlags(frameItem->flags() ^ Qt::ItemIsSelectable);
		frameItem->setIcon(QIcon::fromTheme("no_check"));
    framesTable->setItem(row,0,frameItem);
		QTableWidgetItem* annotationItem = new QTableWidgetItem();
		getAnnotationCounts(row, m_annotatedCounts[row], m_totalCount);
		annotationItem->setText("("+ QString::number(m_annotatedCounts[row]) +
														"/" + QString::number(m_totalCount) + ")");
		annotationItem->setTextAlignment(Qt::AlignCenter);
		annotationItem->setFlags(annotationItem->flags() ^ Qt::ItemIsEditable);
		annotationItem->setFlags(annotationItem->flags() ^ Qt::ItemIsSelectable);
    framesTable->setItem(row,1,annotationItem);
		row++;
	}
	int h = framesTable->horizontalHeader()->height() + 4;
	for (int i = 0; i < framesTable->rowCount(); i++)
		 h += framesTable->rowHeight(i);
	framesTable->setMaximumSize(10000,h);
	selectionChangedSlot(0,0);
	m_currentImgSetIndex = 0;
	frameSetEdit->setText(QString::number(m_currentImgSetIndex+1));
	totalFrameSetLabel->setText(QString::number(Dataset::dataset->imgSets().length()));
}


void DatasetControlWidget::getAnnotationCounts(int frameIndex,
			int &annotatedCount, int &totalCount) {
	totalCount = Dataset::dataset->imgSets()[m_currentImgSetIndex]->
							 frames[frameIndex]->keypoints.size();
	annotatedCount = 0;
	for (const auto & keypoint : Dataset::dataset->imgSets()[m_currentImgSetIndex]->
															 frames[frameIndex]->keypoints) {
		if (keypoint->state() !=  NotAnnotated) annotatedCount++;
	}
}


void DatasetControlWidget::selectionChangedSlot(int row,int) {
	for (int i = 0; i < framesTable->rowCount(); i++) {
		if (i == row) {
			framesTable->item(i,0)->setFont(QFont("Sans Serif", 11, QFont::Bold));
			framesTable->item(i,1)->setFont(QFont("Sans Serif", 11, QFont::Bold));
		}
		else {
			framesTable->item(i,0)->setFont(QFont("Sans Serif", 11));
			framesTable->item(i,1)->setFont(QFont("Sans Serif", 11));
		}
	}
	emit frameSelectionChanged(row);
}


void DatasetControlWidget::frameChangedSlot(int imgSetIndex, int frameIndex) {
	m_currentImgSetIndex = imgSetIndex;
	m_currentFrameIndex = frameIndex;
	for (int i = 0; i < framesTable->rowCount(); i++) {
		if (i == frameIndex) {
			framesTable->item(i,0)->setIcon(QIcon::fromTheme("start"));
			framesTable->item(i,0)->setFont(QFont("Sans Serif", 11, QFont::Bold));
			framesTable->item(i,1)->setFont(QFont("Sans Serif", 11, QFont::Bold));
		}
		else {
			framesTable->item(i,0)->setIcon(QIcon::fromTheme("no_check"));
			framesTable->item(i,0)->setFont(QFont("Sans Serif", 11));
			framesTable->item(i,1)->setFont(QFont("Sans Serif", 11));
		}
		getAnnotationCounts(i, m_annotatedCounts[i], m_totalCount);
		framesTable->item(i,1)->setText("("+ QString::number(m_annotatedCounts[i]) +
																		"/" + QString::number(m_totalCount) + ")");
	}
	frameSetEdit->setText(QString::number(m_currentImgSetIndex+1));
	totalFrameSetLabel->setText(QString::number(Dataset::dataset->imgSets().length()));
}


void DatasetControlWidget::keypointStateChangedSlot(KeypointState state,
			KeypointState previousSate, int frameIndex) {
	if (state == NotAnnotated) {
		m_annotatedCounts[frameIndex]--;
	}
	else {
		if (previousSate != Reprojected) {
			m_annotatedCounts[frameIndex]++;
		}
	}
	framesTable->item(frameIndex,1)->setText("("+ QString::number(m_annotatedCounts[frameIndex]) +
																					 "/" + QString::number(m_totalCount) + ")");
}


void DatasetControlWidget::imgSetChangedSlot() {
	emit imgSetChanged(m_currentImgSetIndex);
}

void DatasetControlWidget::segmentChangedSlot(const QString& segment) {
	Dataset::dataset->save();
	QList<QString> segmentNames = Dataset::dataset->segmentNames();
	QList<QString> cameraNames = Dataset::dataset->cameraNames();
	QList<SkeletonComponent> skeleton = Dataset::dataset->skeleton();
	QString datasetFolder = Dataset::dataset->datasetFolder();
	QString datasetBaseFolder = Dataset::dataset->datasetBaseFolder();
	//datasetFolder = datasetFolder.remove(m_currentSegment);
	datasetFolder = datasetBaseFolder +"/" + segment;
	Dataset::dataset = new Dataset(datasetFolder, datasetBaseFolder,
	 								 		cameraNames, skeleton, segmentNames);
	m_currentSegment = segment;
	m_currentImgSetIndex = 0;
	frameSetEdit->setText(QString::number(m_currentImgSetIndex+1));
	totalFrameSetLabel->setText(QString::number(Dataset::dataset->imgSets().length()));
	emit datasetLoaded();
}
