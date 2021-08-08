/*------------------------------------------------------------
 *  exportdatasetwidget.hpp
 *  Created: 30. July 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef EXPORTDATASETWIDGET_H
#define EXPORTDATASETWIDGET_H

#include "globals.hpp"
#include "dataset.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QErrorMessage>


class ExportDatasetWidget : public QWidget {
	Q_OBJECT
	public:
		explicit ExportDatasetWidget(QWidget *parent = nullptr);

	public slots:

	signals:

	private:
		QSettings *settings;
		QErrorMessage *m_errorMsg;

		private slots:

};


#endif
