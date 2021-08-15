/*------------------------------------------------------------
 *  paircreatorwindow.hpp
 *  Created: 01. August 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef PAIRCREATORWINDOW_H
#define PAIRCREATORWINDOW_H

#include "globals.hpp"
#include "labelwithtooltip.hpp"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QDialog>


class PairCreatorWindow : public QDialog {
	Q_OBJECT
	public:
		explicit PairCreatorWindow(QList<QString> cameraNames, QWidget *parent = nullptr);
		QList<QString> getCameraPair();
		void setCameraPair(QList<QString> cameraPair);

	private:
		int m_mode = 0;
		QRadioButton *twoCamButton;
		QRadioButton *threeCamButton;

		QComboBox *primaryCombo;
		QLabel *intermediateLabel;
		QComboBox *intermediateCombo;
		QComboBox *secondaryCombo;

		QPushButton *addButtom;
		QPushButton *cancelButton;

	private slots:
		void modeToggledSlot(bool toggle);
		void cancelClickedSlot();
};


#endif
