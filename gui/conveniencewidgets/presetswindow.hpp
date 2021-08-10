/*------------------------------------------------------------
 *  presetswindow.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef PRESETSWINDOW_H
#define PRESETSWINDOW_H

#include "globals.hpp"

#include <QToolButton>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QToolBar>
#include <QDialog>

class PresetsWindow : public QDialog {
	Q_OBJECT
	public:
		explicit PresetsWindow(QList<QString> *presets, const QString& type = "load",
					const QString& name = "default", QWidget *parent = nullptr);

	private:
		QSettings *settings;
		QList<QString> *m_presets;
		QString m_name;
		int m_selectedRow = 0;
		QString m_type;

		QWidget *bottomFiller;
		QToolBar *loadToolBar;
		QToolBar *saveToolBar;
		QToolButton *discardButton;
		QAction *discardAction;
		QLineEdit *selectedPresetEdit;
		QLineEdit *newPresetEdit;
		QToolButton *saveButton;
		QAction *saveAction;
		QListWidget *presetsList;
		QPushButton *loadButton;
		QPushButton *closeButton;


		void addItem(const QString &text);

	public slots:
		void updateListSlot();

	signals:
		void savePreset(const QString& name);
		void loadPreset(const QString& name);

	private slots:
		void discardClickedSlot();
		void loadClickedSlot();
		void saveClickedSlot();
		void currentPresetChangedSlot (int row);
		void presetClickedSlot(QListWidgetItem *item);
		void presetNameEditedSlot(const QString& name);
		void currentItemChangedSlot(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif
