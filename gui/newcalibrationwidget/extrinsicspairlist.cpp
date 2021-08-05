/*****************************************************************
 * File:			extrinsicspairlist.cpp
 * Created: 	01. August 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "extrinsicspairlist.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>


ExtrinsicsPairList::ExtrinsicsPairList(QString name, QWidget *parent) :
			m_name(name), QWidget(parent) {

	QGridLayout *labelselectorlayout = new QGridLayout(this);
	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	connect(itemSelectorList, &QListWidget::currentItemChanged, this, &ExtrinsicsPairList::currentItemChangedSlot);
	moveItemUpButton = new QPushButton();
	moveItemUpButton->setIcon(QIcon::fromTheme("up"));
	moveItemUpButton->setMinimumSize(35,35);
	connect(moveItemUpButton, &QPushButton::clicked, this, &ExtrinsicsPairList::moveItemUpSlot);
	moveItemDownButton = new QPushButton();
	moveItemDownButton->setIcon(QIcon::fromTheme("down"));
	moveItemDownButton->setMinimumSize(35,35);
	connect(moveItemDownButton, &QPushButton::clicked, this, &ExtrinsicsPairList::moveItemDownSlot);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &ExtrinsicsPairList::addItemSlot);
	deleteItemButton = new QPushButton();
	deleteItemButton->setIcon(QIcon::fromTheme("discard"));
	deleteItemButton->setMinimumSize(35,35);
	connect(deleteItemButton, &QPushButton::clicked, this, &ExtrinsicsPairList::removeItemSlot);
	labelselectorlayout->addWidget(itemSelectorList,0,0,1,4);
	labelselectorlayout->addWidget(moveItemUpButton,1,0);
	labelselectorlayout->addWidget(moveItemDownButton,1,1);
	labelselectorlayout->addWidget(addItemButton,1,2);
	labelselectorlayout->addWidget(deleteItemButton,1,3);
	this->setLayout(labelselectorlayout);
}

QList<QList<QString>> ExtrinsicsPairList::getItems() {
	return m_cameraPairs;
}

void ExtrinsicsPairList::setItems(QList<QList<QString>> items) {
	m_cameraPairs.clear();
	itemSelectorList->clear();
	for (const auto & newPair : items) {
		if (newPair.size() == 2) {
			m_cameraPairs.append(newPair);
			addItem(newPair[0] + " --> " + newPair[1]);
		}
		else if (newPair.size() == 3) {
			m_cameraPairs.append(newPair);
			addItem(newPair[0] + " --> " + newPair[1] + " --> " + newPair[2]);
		}
	}
	itemSelectorList->setCurrentRow(0);
}

void ExtrinsicsPairList::moveItemUpSlot() {
	int row = itemSelectorList->currentRow();
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	QListWidgetItem *seperatorItem = itemSelectorList->takeItem(row);
	int newRow = std::max(row-2,0);
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->insertItem(newRow+1,seperatorItem);
	m_cameraPairs.move(itemSelectorList->currentRow()/2, newRow/2);
	itemSelectorList->setCurrentRow(newRow);

}

void ExtrinsicsPairList::moveItemDownSlot() {
	int row = itemSelectorList->currentRow();
	QListWidgetItem *item = itemSelectorList->takeItem(row);
	QListWidgetItem *seperatorItem = itemSelectorList->takeItem(row);
	int newRow = std::min(row+2,itemSelectorList->count());
	itemSelectorList->insertItem(newRow,item);
	itemSelectorList->insertItem(newRow+1,seperatorItem);
	m_cameraPairs.move(itemSelectorList->currentRow()/2, newRow/2);
	itemSelectorList->setCurrentRow(newRow);
}


void ExtrinsicsPairList::addItemSlot() {
	PairCreatorWindow* pairCreatorWindow = new PairCreatorWindow(m_cameraNames, this);
	pairCreatorWindow->exec();
	QList<QString> newPair = pairCreatorWindow->getCameraPair();
	if (newPair.size() == 2) {
		m_cameraPairs.append(newPair);
		addItem(newPair[0] + " --> " + newPair[1]);
	}
	else if (newPair.size() == 3) {
		m_cameraPairs.append(newPair);
		addItem(newPair[0] + " --> " + newPair[1] + " --> " + newPair[1]);
	}
}

void ExtrinsicsPairList::removeItemSlot() {
	delete itemSelectorList->takeItem(itemSelectorList->currentRow());
	delete itemSelectorList->takeItem(itemSelectorList->currentRow()+1);
	m_cameraPairs.removeAt(itemSelectorList->currentRow()/2);
}

void ExtrinsicsPairList::cameraNamesChangedSlot(QList<QString> cameraNames) {
	m_cameraNames = cameraNames;
}

void ExtrinsicsPairList::addItem(const QString &text) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
	itemSelectorList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	itemSelectorList->addItem(seperatorItem);
}

void ExtrinsicsPairList::currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous) {
	if (current != nullptr)  {
		current->setBackgroundColor(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackgroundColor(QColor(34, 36, 40));
	}
}


PairCreatorWindow::PairCreatorWindow(QList<QString> cameraNames, QWidget *parent) : QDialog(parent) {
	this->setWindowTitle("Create Camera Pair");
	this->setMinimumSize(600,250);
	QGridLayout *layout = new QGridLayout(this);
	layout->setHorizontalSpacing(20);
	QWidget *mainWidget = new QWidget(this);
	QGridLayout *mainlayout = new QGridLayout(mainWidget);
	mainlayout->setMargin(0);
	mainlayout->setVerticalSpacing(0);
 	LabelWithToolTip *modeLabel = new LabelWithToolTip("Mode");
	twoCamButton = new QRadioButton("2 Cameras",mainWidget);
	twoCamButton->setChecked(true);
	connect(twoCamButton, &QRadioButton::toggled, this, &PairCreatorWindow::modeToggledSlot);
	threeCamButton = new QRadioButton("3 Cameras",mainWidget);
	QGroupBox *selectorBox = new QGroupBox(mainWidget);
	QGridLayout *selectorlayout = new QGridLayout(selectorBox);
	QLabel *primaryLabel = new QLabel("Primary Camera");
	primaryCombo = new QComboBox();
	intermediateLabel = new QLabel("Intermediate Camera");
	intermediateCombo = new QComboBox();
	intermediateLabel->hide();
	intermediateCombo->hide();
	QLabel *secondaryLabel = new QLabel("Secondary Camera");
	secondaryCombo = new QComboBox();
	for (const auto & cam : cameraNames) {
		primaryCombo->addItem(cam);
		intermediateCombo->addItem(cam);
		secondaryCombo->addItem(cam);
	}

	selectorlayout->addWidget(primaryLabel,0,0);
	selectorlayout->addWidget(primaryCombo,0,1);
	selectorlayout->addWidget(intermediateLabel,1,0);
	selectorlayout->addWidget(intermediateCombo,1,1);
	selectorlayout->addWidget(secondaryLabel,2,0);
	selectorlayout->addWidget(secondaryCombo,2,1);
	mainlayout->addWidget(modeLabel,0,0);
	mainlayout->addWidget(twoCamButton,0,1);
	mainlayout->addWidget(threeCamButton,0,2);
	mainlayout->addWidget(selectorBox,1,0,1,3);

	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spacer->setMaximumSize(1000,50);
	cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon(QIcon::fromTheme("discard"));
	cancelButton->setMinimumSize(40,40);
	connect(cancelButton, &QPushButton::clicked, this, &PairCreatorWindow::accept);
	addButtom = new QPushButton("Add");
	addButtom->setIcon(QIcon::fromTheme("add"));
	addButtom->setMinimumSize(40,40);
	connect(addButtom, &QPushButton::clicked, this, &PairCreatorWindow::reject);

	layout->addWidget(mainWidget,0,0,1,3);
	layout->addWidget(spacer,1,0);
	layout->addWidget(cancelButton,1,1);
	layout->addWidget(addButtom,1,2);
}

void PairCreatorWindow::modeToggledSlot(bool toggle) {
	if (toggle) {
		intermediateLabel->hide();
		intermediateCombo->hide();
		m_mode = 0;
	}
	else {
		intermediateLabel->show();
		intermediateCombo->show();
		m_mode = 1;
	}
}

void PairCreatorWindow::cancelClickedSlot() {
	this->close();
}

QList<QString> PairCreatorWindow::getCameraPair() {
	QList<QString> pair;
	QString primary = primaryCombo->currentText();
	QString intermediate = intermediateCombo->currentText();
	QString secondary = secondaryCombo->currentText();

	if (m_mode == 0) {
		if (primary != "" && secondary != "" && (primary != secondary)) {
			pair.append(primaryCombo->currentText());
			pair.append(secondaryCombo->currentText());
		}
	}
	else if (m_mode == 1) {
		if (primary != "" && intermediate != "" && secondary != "" && (primary != intermediate) && (primary != secondary) && (secondary != intermediate)) {
			pair.append(primaryCombo->currentText());
			pair.append(intermediateCombo->currentText());
			pair.append(secondaryCombo->currentText());
		}
	}
	return pair;
}
