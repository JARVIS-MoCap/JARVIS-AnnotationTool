/*------------------------------------------------------------
 *  datasetcontrolwidget.cpp
 *  Created:  03. December 2020
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "datasetcontrolwidget.hpp"

#include <QHeaderView>


DatasetControlWidget::DatasetControlWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	QLabel *datasetControlLabel = new QLabel("Dataset Control");
	datasetControlLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));

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

	layout->addWidget(datasetControlLabel,0,0);
	layout->addWidget(framesTable,1,0);
	layout->addWidget(spacer,3,0);

	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals

	//<-> Relayed Signals
}

void DatasetControlWidget::datasetLoadedSlot() {
	framesTable->setRowCount(Dataset::dataset->numCameras());
	m_annotatedCounts = new int [Dataset::dataset->numCameras()];
	int row = 0;
	for (const auto& cameraName : Dataset::dataset->cameraNames()) {
    QTableWidgetItem* frameItem = new QTableWidgetItem();
    frameItem->setText(cameraName);
		frameItem->setFlags(frameItem->flags() ^ Qt::ItemIsEditable);
		frameItem->setFlags(frameItem->flags() ^ Qt::ItemIsSelectable);
		frameItem->setIcon(QIcon::fromTheme("no_check"));
    framesTable->setItem(row,0,frameItem);
		QTableWidgetItem* annotationItem = new QTableWidgetItem();
		getAnnotationCounts(row, m_annotatedCounts[row], m_totalCount);
		annotationItem->setText("("+ QString::number(m_annotatedCounts[row]) +  "/" + QString::number(m_totalCount) + ")");
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
}

void DatasetControlWidget::getAnnotationCounts(int frameIndex, int &annotatedCount, int &totalCount) {
	totalCount = Dataset::dataset->imgSets()[m_currentImgSetIndex]->frames[frameIndex]->keypoints.size();
	annotatedCount = 0;
	for (const auto & keypoint : Dataset::dataset->imgSets()[m_currentImgSetIndex]->frames[frameIndex]->keypoints) {
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
		framesTable->item(i,1)->setText("("+ QString::number(m_annotatedCounts[i]) +  "/" + QString::number(m_totalCount) + ")");
	}
}

void DatasetControlWidget::keypointStateChangedSlot(KeypointState state, KeypointState previousSate, int frameIndex) {
	if (state == NotAnnotated) {
		m_annotatedCounts[frameIndex]--;
	}
	else {
		if (previousSate != Reprojected) {
			m_annotatedCounts[frameIndex]++;
		}
	}
	framesTable->item(frameIndex,1)->setText("("+ QString::number(m_annotatedCounts[frameIndex]) +  "/" + QString::number(m_totalCount) + ")");
}

void DatasetControlWidget::imgSetChangedSlot() {
	emit imgSetChanged(m_currentImgSetIndex);
}
