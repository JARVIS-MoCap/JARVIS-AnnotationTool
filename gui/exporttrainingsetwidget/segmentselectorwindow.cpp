/*****************************************************************
 * File:			segmentselectorwindow.cpp
 * Created: 	14. August 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "segmentselectorwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>


SegmentSelectorWindow::SegmentSelectorWindow(QList<QPair<QString, bool>> &subSets, QWidget *parent) :
			QDialog(parent), m_subSets(subSets) {
	this->setWindowTitle("Select Segments");
	this->setMinimumSize(600,300);
	QGridLayout *layout = new QGridLayout(this);

	QGroupBox *segmentBox = new QGroupBox("Segments", this);
	QGridLayout *segementlayout = new QGridLayout(segmentBox);
	subSetList = new CheckableItemList("Segments", this);
	connect(subSetList, &CheckableItemList::checkStateChanged, this, &SegmentSelectorWindow::checkStateChangedSlot);
	for (const auto &subSet : m_subSets) {
		subSetList->addItem(subSet.first, subSet.second);
	}
	segementlayout->addWidget(subSetList,0,0);

	closeButton = new QPushButton("Close", this);
	closeButton->setMinimumSize(30,30);
	connect(closeButton, &QPushButton::clicked, this, &SegmentSelectorWindow::accept);


	layout->addWidget(segmentBox,0,0);
	layout->addWidget(closeButton, 1,0, Qt::AlignRight);

}

void SegmentSelectorWindow::checkStateChangedSlot(int index, bool state) {
	m_subSets[index].second = state;
}
