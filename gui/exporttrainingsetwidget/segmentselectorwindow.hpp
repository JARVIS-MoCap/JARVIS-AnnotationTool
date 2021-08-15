/*------------------------------------------------------------
 *  segmentselectorwindow.hpp
 *  Created: 14. August 2021
 *  Author:   Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *------------------------------------------------------------*/

#ifndef SEGMENTSELECTORWINDOW_H
#define SEGMENTSELECTORWINDOW_H

#include "globals.hpp"
#include "checkableitemlist.hpp"

#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QListWidget>
#include <QDialog>


class SegmentSelectorWindow : public QDialog {
	Q_OBJECT

	public:
		explicit SegmentSelectorWindow(QList<QPair<QString, bool>> &subSets, QWidget *parent = nullptr);

	private:
		QList<QPair<QString, bool>> &m_subSets;

		CheckableItemList *subSetList;
		QPushButton *closeButton;

	private slots:
		void checkStateChangedSlot(int index, bool state);
};


#endif
