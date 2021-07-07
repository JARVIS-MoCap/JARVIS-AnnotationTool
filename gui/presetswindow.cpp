/*------------------------------------------------------------
 *  presetswindow.cpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#include "presetswindow.hpp"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QToolButton>
#include <QMessageBox>

PresetsWindow::PresetsWindow(QList<QString> *presets, const QString& type, const QString& name, QWidget *parent)
			: QWidget(parent, Qt::Window), m_presets{presets}, m_name{name} {
	this->resize(600,400);
	this->setMinimumSize(600,200);
	settings = new QSettings();
	if (type == "load") setWindowTitle("Load Preset");
	else setWindowTitle("Save Preset");
	m_selectedRow = -1;
	*m_presets = settings->value(m_name + "/Presets").value<QList<QString>>();
	QGridLayout *layout = new QGridLayout(this);
	bottomFiller = new QWidget(this);
	bottomFiller->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	loadToolBar = new QToolBar(this);
	loadToolBar->setFixedHeight(45);
	loadToolBar->setMovable(false);

	discardButton = new QToolButton(loadToolBar);
	discardAction = new QAction(loadToolBar);
	createToolBarButton(discardButton, discardAction, QIcon::fromTheme("discard"), false,
				false, QSize(40,40));
	connect(discardAction, &QAction::triggered, this, &PresetsWindow::discardClickedSlot);
	loadButton = new QToolButton(loadToolBar);
	loadAction = new QAction(loadToolBar);
	loadAction->setIcon(QIcon("icons/download.png"));
	createToolBarButton(loadButton, loadAction, QIcon::fromTheme("download"), false,
				false, QSize(40,40));
	connect(loadAction, &QAction::triggered, this, &PresetsWindow::loadClickedSlot);
	loadToolBar->addWidget(discardButton);
	loadToolBar->addWidget(loadButton);

	saveToolBar = new QToolBar(this);
	saveToolBar->setFixedHeight(45);
	saveToolBar->setMovable(false);
	QLabel *newPresetLabel = new QLabel("Preset Name: ", saveToolBar);
	newPresetEdit = new QLineEdit(saveToolBar);
	newPresetEdit->setPlaceholderText("Enter Name...");
	connect (newPresetEdit, SIGNAL(textEdited(QString)), this, SLOT(presetNameEditedSlot(QString)));
	connect(newPresetEdit, SIGNAL(returnPressed()), this, SLOT(saveClickedSlot()));
	saveButton = new QToolButton(saveToolBar);
	saveAction = new QAction(saveToolBar);
	createToolBarButton(saveButton, saveAction, QIcon::fromTheme("save"), false,
				false, QSize(40,40));
	connect(saveAction, &QAction::triggered, this, &PresetsWindow::saveClickedSlot);
	saveToolBar->addWidget(newPresetLabel);
	saveToolBar->addWidget(newPresetEdit);
	saveToolBar->addWidget(saveButton);

	presetsTable = new QTableWidget(0, 1);
	presetsTable->setAlternatingRowColors(true);
	QStringList labels;
	labels << "Presets";
	presetsTable->setHorizontalHeaderLabels(labels);
	presetsTable->horizontalHeader()-> setSectionResizeMode(0, QHeaderView::Stretch);
	presetsTable->verticalHeader()->hide();
	presetsTable->setShowGrid(false);
	presetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(presetsTable, SIGNAL(itemSelectionChanged()), this, SLOT(rowChangedSlot()));
	QPushButton *quitButton = new QPushButton("Quit");
	//quitButton->setFont(fonts["bold"]);
	quitButton->setMaximumSize(100,35);
	quitButton->setMinimumSize(100,35);
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));


	if(type == "load") {
		layout->setMenuBar(loadToolBar);
		saveToolBar->hide();
	}
	else {
		layout->setMenuBar(saveToolBar);
		loadToolBar->hide();
	}
	layout->addWidget(presetsTable,0,0);
	layout->addWidget(quitButton,1,0,1,1,Qt::AlignRight);
	this->setLayout(layout);
}

void PresetsWindow::updateListSlot() {
	if (newPresetEdit->text() != "") saveAction->setEnabled(true);
	presetsTable->setRowCount(m_presets->size());
	int row = 0;
	for (const auto& preset : *m_presets) {
			QTableWidgetItem* presetItem = new QTableWidgetItem();
		presetItem->setText(preset);
		presetItem->setFlags(presetItem->flags() ^ Qt::ItemIsEditable);
		presetsTable->setItem(row,0,presetItem);
		row++;
	}
	discardAction->setEnabled(false);
	loadAction->setEnabled(false);
}

void PresetsWindow::loadClickedSlot() {
	emit loadPreset(m_name + "/" + m_presets->value(presetsTable->currentRow()));
	close();
}

void PresetsWindow::discardClickedSlot() {
  QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "", "Delete this Preset?",
                                QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::No) {
    return;
  }
	settings->remove(m_name + "/" + m_presets->value(presetsTable->currentRow()));
	QList<QString>::iterator it = m_presets->begin();
	it += presetsTable->currentRow();
	m_presets->erase(it);
	presetsTable->removeRow(m_selectedRow);
	discardAction->setEnabled(false);
	loadAction->setEnabled(false);
	settings->setValue(m_name + "/Presets", QVariant::fromValue(*m_presets));
	m_selectedRow = -1;
}


void PresetsWindow::saveClickedSlot() {
	if (newPresetEdit->text() != "" && m_presets->indexOf(newPresetEdit->text()) == -1) {
		m_presets->push_back(newPresetEdit->text());
	}
	updateListSlot();
	settings->setValue(m_name + "/Presets", QVariant::fromValue(*m_presets));
	emit savePreset(m_name + "/" + newPresetEdit->text());
	newPresetEdit->setText("");
}


void PresetsWindow::presetNameEditedSlot(const QString& name) {
	saveAction->setEnabled(name != "");
}


void PresetsWindow::rowChangedSlot() {
	if (presetsTable->currentRow() == m_selectedRow) {
		discardAction->setEnabled(false);
		loadAction->setEnabled(false);
		m_selectedRow = -1;
	}
	else {
		discardAction->setEnabled(true);
		loadAction->setEnabled(true);
		m_selectedRow = presetsTable->currentRow();
	}
}
