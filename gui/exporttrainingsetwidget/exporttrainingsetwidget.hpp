/*------------------------------------------------------------
 *  exporttrainingsetwidget.hpp
 *  Created: 30. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef EXPORTTRAININGSETWIDGET_H
#define EXPORTTRAININGSETWIDGET_H

#include "globals.hpp"
#include "presetswindow.hpp"


#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QErrorMessage>


class ExportTrainingsetWidget : public QWidget {
	Q_OBJECT
	public:
		explicit ExportTrainingsetWidget(QWidget *parent = nullptr);

	signals:

	public slots:


	private:
		QSettings *settings;
		QErrorMessage *m_errorMsg;
		PresetsWindow *loadPresetsWindow;
		PresetsWindow *savePresetsWindow;
		QList<QString> presets;

		QPushButton *saveButton;
		QPushButton *loadButton;
		QPushButton *exportButton;

		private slots:
		void savePresetsClickedSlot();
		void loadPresetsClickedSlot();
		void savePresetSlot(const QString& preset);
		void loadPresetSlot(const QString& preset);

};


#endif
