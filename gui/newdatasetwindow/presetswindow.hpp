/*------------------------------------------------------------
 *  presetswindow.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef PRESETSWINDOW_H
#define PRESETSWINDOW_H

#include "globals.hpp"

#include <QToolButton>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QToolBar>

class PresetsWindow : public QWidget {
	Q_OBJECT
	public:
		explicit PresetsWindow(QList<QString> *presets, const QString& type = "load",
					const QString& name = "default", QWidget *parent = nullptr);

	private:
		QSettings *settings;
		QList<QString> *m_presets;
		QString m_name;
		int m_selectedRow = 0;

		QWidget *bottomFiller;
		QToolBar *loadToolBar;
		QToolBar *saveToolBar;
		QToolButton *discardButton;
		QAction *discardAction;
		QToolButton *loadButton;
		QAction *loadAction;
		QLineEdit *currentPreset;
		QLineEdit *newPresetEdit;
		QToolButton *saveButton;
		QAction *saveAction;
		QTableWidget *presetsTable;

	public slots:
		void updateListSlot();

	signals:
		void savePreset(const QString& name);
		void loadPreset(const QString& name);

	private slots:
		void discardClickedSlot();
		void loadClickedSlot();
		void saveClickedSlot();
		void rowChangedSlot();
		void presetNameEditedSlot(const QString& name);
};

#endif
