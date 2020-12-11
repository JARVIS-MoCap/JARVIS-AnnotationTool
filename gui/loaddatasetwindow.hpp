/*------------------------------------------------------------
 *  loaddatasetwindow.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef LOADDATASETWINDOW_H
#define LOADDATASETWINDOW_H

#include "globals.hpp"
#include "dataset.hpp"

#include <QToolButton>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QToolBar>
#include <QGroupBox>

class LoadDatasetWindow : public QWidget {
	Q_OBJECT
	public:
		explicit LoadDatasetWindow(QWidget *parent = nullptr);

	public slots:

	signals:
		void datasetLoaded();

	private:
		QSettings *settings;

		QGroupBox *datasetFolderBox;
		QLineEdit *datasetFolderEdit;
		QPushButton *datasetFolderButton;
		QPushButton *loadDatasetButton;


		private slots:
			void datasetFolderClickedSlot();
			void loadDatasetClickedSlot();
};

#endif
