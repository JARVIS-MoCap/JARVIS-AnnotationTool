/*****************************************************************
	* File:			  presetswindow.cpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "presetswindow.hpp"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QToolButton>
#include <QMessageBox>
#include <QGroupBox>

PresetsWindow::PresetsWindow(QList<QString> *presets, const QString& type, const QString& name, QWidget *parent)
			:  QDialog(parent), m_presets{presets}, m_type(type), m_name{name} {
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
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

	QLabel *selectedPresetLabel = new QLabel("Preset Name: ", loadToolBar);
	selectedPresetEdit = new QLineEdit(loadToolBar);
	selectedPresetEdit->setPlaceholderText("Select a Preset...");
	selectedPresetEdit->setStyleSheet("background-color:palette(Window);");
	selectedPresetEdit->setReadOnly(true);
	discardButton = new QToolButton(loadToolBar);
	discardAction = new QAction(loadToolBar);
	createToolBarButton(discardButton, discardAction, QIcon::fromTheme("discard"), false,
				false, QSize(40,40));
	connect(discardAction, &QAction::triggered, this, &PresetsWindow::discardClickedSlot);
	loadToolBar->addWidget(selectedPresetLabel);
	loadToolBar->addWidget(selectedPresetEdit);
	loadToolBar->addWidget(discardButton);

	saveToolBar = new QToolBar(this);
	saveToolBar->setFixedHeight(45);
	saveToolBar->setMovable(false);
	QLabel *newPresetLabel = new QLabel("Preset Name: ", saveToolBar);
	newPresetEdit = new QLineEdit(saveToolBar);
	newPresetEdit->setPlaceholderText("Enter Name...");
	newPresetEdit->setStyleSheet("background-color:palette(Window);");
	connect(newPresetEdit, &QLineEdit::textEdited, this, &PresetsWindow::presetNameEditedSlot);
	connect(newPresetEdit, &QLineEdit::returnPressed, this, &PresetsWindow::saveClickedSlot);
	saveButton = new QToolButton(saveToolBar);
	saveAction = new QAction(saveToolBar);
	createToolBarButton(saveButton, saveAction, QIcon::fromTheme("save"), false,
				false, QSize(40,40));
	connect(saveAction, &QAction::triggered, this, &PresetsWindow::saveClickedSlot);
	saveToolBar->addWidget(newPresetLabel);
	saveToolBar->addWidget(newPresetEdit);
	saveToolBar->addWidget(saveButton);

	QGroupBox *presetsGroup = new QGroupBox("Presets", this);
	QGridLayout *presetslayout = new QGridLayout(presetsGroup);
	presetslayout->setMargin(3);
	presetsList = new QListWidget(this);
	presetslayout->addWidget(presetsList);
	presetsList->setAlternatingRowColors(true);
	presetsList->setFont(QFont("Sans Serif", 12));
	connect(presetsList, &QListWidget::currentRowChanged, this, &PresetsWindow::currentPresetChangedSlot);
	connect(presetsList, &QListWidget::itemClicked, this, &PresetsWindow::presetClickedSlot);
	connect(presetsList, &QListWidget::currentItemChanged, this, &PresetsWindow::currentItemChangedSlot);
	presetsList->setCurrentRow(0);
	closeButton = new QPushButton("Close");
	closeButton->setMaximumSize(100,35);
	closeButton->setMinimumSize(100,35);
	connect(closeButton, &QPushButton::clicked, this, &PresetsWindow::close);
	loadButton = new QPushButton("Load");
	loadButton->setIcon(QIcon::fromTheme("download"));
	loadButton->setMaximumSize(100,35);
	loadButton->setMinimumSize(100,35);
	connect(loadButton, &QPushButton::clicked, this, &PresetsWindow::loadClickedSlot);
	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spacer->setMaximumSize(9999,40);
	if(type == "load") {
		layout->setMenuBar(loadToolBar);
		saveToolBar->hide();
		layout->addWidget(spacer, 1,0);
		layout->addWidget(closeButton,1,1, Qt::AlignRight);
		layout->addWidget(loadButton,1,2, Qt::AlignRight);
	}
	else {
		layout->setMenuBar(saveToolBar);
		loadToolBar->hide();
		layout->addWidget(spacer, 1,0,1,2);
		layout->addWidget(closeButton,1,2,Qt::AlignRight);
	}
	layout->addWidget(presetsGroup,0,0,1,3);
	this->setLayout(layout);
}

void PresetsWindow::updateListSlot() {
	presetsList->clear();
	if (newPresetEdit->text() != "") saveAction->setEnabled(true);
	for (const auto& preset : *m_presets) {
		addItem(preset);
	}
}

void PresetsWindow::loadClickedSlot() {
	emit loadPreset(m_name + "/" + m_presets->value(presetsList->currentRow()/2));
	close();
}

void PresetsWindow::discardClickedSlot() {
  QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "", "Delete this Preset?",
                                QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::No) {
    return;
  }
	settings->remove(m_name + "/" + m_presets->value(presetsList->currentRow()/2));

	QList<QString>::iterator it = m_presets->begin();
	it += presetsList->currentRow()/2;
	m_presets->erase(it);
	delete presetsList->takeItem(m_selectedRow);
	settings->setValue(m_name + "/Presets", QVariant::fromValue(*m_presets));
	m_selectedRow = -1;
}


void PresetsWindow::saveClickedSlot() {
	if (m_presets->indexOf(newPresetEdit->text()) != -1) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "", "Preset \"" + newPresetEdit->text() +
																	"\" already exists! Overwrite this preset?",
																	QMessageBox::Yes|QMessageBox::No);
		if (reply == QMessageBox::No) {
			return;
		}
	}
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


void PresetsWindow::currentPresetChangedSlot(int row) {
	if (row == m_selectedRow) {
		discardAction->setEnabled(false);
		m_selectedRow = -1;
	}
	else {
		discardAction->setEnabled(true);
		m_selectedRow = row;
	}
}

void PresetsWindow::presetClickedSlot(QListWidgetItem *item) {
	if (m_type == "save") {
		newPresetEdit->setText((*m_presets)[presetsList->row(item)/2]);
		if (newPresetEdit->text() != "") saveAction->setEnabled(true);
	}
	else {
		selectedPresetEdit->setText((*m_presets)[presetsList->row(item)/2]);
	}
}


void PresetsWindow::currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous) {
	if (current != nullptr) {
		current->setBackground(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackground(QColor(34, 36, 40));
	}
}

void PresetsWindow::addItem(const QString &text) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
	presetsList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	presetsList->addItem(seperatorItem);
}
