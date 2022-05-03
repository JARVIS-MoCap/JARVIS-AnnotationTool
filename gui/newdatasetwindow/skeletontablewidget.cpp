/*****************************************************************
	* File:			  skeletontablewidget.cpp
	* Created: 	  01. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "skeletontablewidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>
#include <QHeaderView>
#include <QDoubleSpinBox>

SkeletonTableWidget::SkeletonTableWidget(QString name, QWidget *parent) :
			QWidget(parent), m_name(name) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(3);
	skeletonTable = new QTableWidget(0, 4);
	skeletonTable->setSelectionMode(QAbstractItemView::SingleSelection);
	skeletonTable->setFont(QFont("Sans Serif", 12));
	QStringList labels;
	labels << "Name" << "Keypoint A" << "Keypoint B" << "Length [mm]";
	skeletonTable->setHorizontalHeaderLabels(labels);
	skeletonTable->setColumnWidth(0, 160);
	skeletonTable->setColumnWidth(1, 150);
	skeletonTable->setColumnWidth(2, 150);
	skeletonTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	skeletonTable->verticalHeader()->hide();
	skeletonTable->setShowGrid(false);
	skeletonTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect(skeletonTable, &QTableWidget::currentItemChanged, this, &SkeletonTableWidget::currentItemChangedSlot);
	connect(skeletonTable, &QTableWidget::cellDoubleClicked, this, &SkeletonTableWidget::cellDoubleClickedSlot);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &SkeletonTableWidget::addItemSlot);
	deleteItemButton = new QPushButton();
	deleteItemButton->setIcon(QIcon::fromTheme("discard"));
	deleteItemButton->setMinimumSize(35,35);
	connect(deleteItemButton, &QPushButton::clicked, this, &SkeletonTableWidget::removeItemSlot);
	layout->addWidget(skeletonTable,0,0,1,2);
	layout->addWidget(addItemButton,1,0);
	layout->addWidget(deleteItemButton,1,1);
}


QList<SkeletonComponent> SkeletonTableWidget::getItems() {
	QList<SkeletonComponent> items;
	for (int row = 0; row < skeletonTable->rowCount(); row++) {
		SkeletonComponent skeletonComponent;
		QLabel *name = static_cast<QLabel*>(skeletonTable->cellWidget(row,0));
		QComboBox *keypointACombo = static_cast<QComboBox*>(skeletonTable->cellWidget(row,1));
		QComboBox *keypointBCombo = static_cast<QComboBox*>(skeletonTable->cellWidget(row,2));
		QDoubleSpinBox *lengthEdit = static_cast<QDoubleSpinBox*>(skeletonTable->cellWidget(row,3));
		skeletonComponent.name = name->text();
		skeletonComponent.keypointA = keypointACombo->currentText();
		skeletonComponent.keypointB = keypointBCombo->currentText();
		skeletonComponent.length = lengthEdit->value();
		items.append(skeletonComponent);
	}
	return items;
}



void SkeletonTableWidget::setItems(QList<SkeletonComponent> items) {
	skeletonTable->setRowCount(items.length());
	for (int row = 0; row < items.length(); row++) {
		skeletonTable->setCellWidget (row,0, new QLabel(items[row].name));
		QComboBox *keypointACombo = new QComboBox();
		QComboBox *keypointBCombo = new QComboBox();
		QDoubleSpinBox *lengthBox = new QDoubleSpinBox(skeletonTable);
		lengthBox->setRange(0,10000);
		skeletonTable->setCellWidget(row,1, keypointACombo);
		skeletonTable->setCellWidget(row,2, keypointBCombo);
		skeletonTable->setCellWidget(row,3, lengthBox);
		int idx = 0;
		for (const auto& keypoint : m_keypointList) {
			keypointACombo->addItem(keypoint);
			if (idx != 0) {
				keypointBCombo->addItem(keypoint);
			}
			idx++;
	}
		keypointACombo->setCurrentText(items[row].keypointA);
		keypointBCombo->setCurrentText(items[row].keypointB);
		lengthBox->setValue(items[row].length);
	}
	m_itemCounter = items.length() + 1;
}



void SkeletonTableWidget::addItemSlot() {
	skeletonTable->setRowCount(skeletonTable->rowCount()+1);
	skeletonTable->setCellWidget (skeletonTable->rowCount()-1,0, new QLabel("Joint " + QString::number(m_itemCounter++)));
	QComboBox *keypointACombo = new QComboBox();
	QComboBox *keypointBCombo = new QComboBox();
	QDoubleSpinBox *lengthBox = new QDoubleSpinBox(skeletonTable);
	lengthBox->setRange(0,10000);
	skeletonTable->setCellWidget(skeletonTable->rowCount()-1,1, keypointACombo);
	skeletonTable->setCellWidget(skeletonTable->rowCount()-1,2, keypointBCombo);
	skeletonTable->setCellWidget(skeletonTable->rowCount()-1,3, lengthBox);
	for (const auto& keypoint : m_keypointList) {
		keypointACombo->addItem(keypoint);
		keypointBCombo->addItem(keypoint);
	}
	keypointACombo->setCurrentIndex(0);
	keypointBCombo->setCurrentIndex(1);
}



void SkeletonTableWidget::removeItemSlot() {
	skeletonTable->removeRow(skeletonTable->currentRow());
	skeletonTable->setCurrentCell(skeletonTable->currentRow(),0);
}


void SkeletonTableWidget::currentItemChangedSlot(QTableWidgetItem *current,
			QTableWidgetItem *previous) {
	if (current != nullptr)  {
		current->setBackground(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackground(QColor(34, 36, 40));
	}
}

void SkeletonTableWidget::setKeypointsListSlot(QList<QString> keypointsList) {
	m_keypointList = keypointsList;
	updateComboBoxes();
}

void SkeletonTableWidget::updateComboBoxes() {
	for (int row = 0; row < skeletonTable->rowCount(); row++) {
		QComboBox *keypointACombo = static_cast<QComboBox*>(skeletonTable->cellWidget(row,1));
		QComboBox *keypointBCombo = static_cast<QComboBox*>(skeletonTable->cellWidget(row,2));
		QString oldKeypointA = keypointACombo->currentText();
		QString oldKeypointB = keypointBCombo->currentText();
		keypointACombo->clear();
		keypointBCombo->clear();
		for (const auto& keypoint : m_keypointList) {
			keypointACombo->addItem(keypoint);
			keypointBCombo->addItem(keypoint);
		}
		if (m_keypointList.contains(oldKeypointA)) {
			keypointACombo->setCurrentText(oldKeypointA);
		}
		else  {
			keypointACombo->setCurrentIndex(0);
		}
		if (m_keypointList.contains(oldKeypointB)) {
			keypointBCombo->setCurrentText(oldKeypointB);
		}
		else  {
			keypointBCombo->setCurrentIndex(1);
			//keypointBCombo->setStyleSheet("color: rgb(164,100,32); ");
		}
	}
}

void SkeletonTableWidget::cellDoubleClickedSlot(int row, int column) {
	QLabel *nameLabel = static_cast<QLabel*>(skeletonTable->cellWidget(row, column));
	bool ok;
	QString text = QInputDialog::getText(this, tr("Enter Skeleton Bone Name:"),
																			 tr("Name:"), QLineEdit::Normal,
																			 nameLabel->text(), &ok);
	if (ok && !text.isEmpty())
			nameLabel->setText(text);
}
