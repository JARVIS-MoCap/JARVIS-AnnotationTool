/*****************************************************************
	* File:			  keypointwidget.hpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#ifndef KEYPOINTWIDGET_H
#define KEYPOINTWIDGET_H

#include "globals.hpp"
#include "colormap.hpp"
#include "keypoint.hpp"
#include "dataset.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QToolBar>
#include <QPushButton>
#include <QCheckBox>
#include <QTabWidget>
#include <QListWidget>
#include <QMenu>
#include <QKeyEvent>

class KeypointListWidget : public QListWidget {
	Q_OBJECT
	public:
		explicit KeypointListWidget() {
			this->installEventFilter(this);
			setContextMenuPolicy(Qt::CustomContextMenu);
			connect(this, &KeypointListWidget::customContextMenuRequested, this, &KeypointListWidget::showContextMenu);
			connect(this, &KeypointListWidget::itemDoubleClicked, this, &KeypointListWidget::itemDoubleClickedSlot);
		}
		void addSupressed(int row) {
			m_suppressedList.append(row);
		}
		void clearSupressed() {
			m_suppressedList.clear();
		}
		// bool isSuppressed(int row) {
			// return m_suppressedList.contains(row);
		// }

		void toggleCurrentKeypointSuppression(){
			int current = this->currentRow();
			bool isSuppressed = m_suppressedList.contains(current);
			if (isSuppressed) {
				m_suppressedList.removeAll(current);
				emit unsuppressKeypoint(current);
			}
			else {
				m_suppressedList.append(current);
				emit suppressKeypoint(current);
			}
			emit afterToggleSuppression();
		}

	signals:
		void removeKeypoint(int row);
		void suppressKeypoint(int row);
		void unsuppressKeypoint(int row);
		void afterToggleSuppression();

	private:
		QList<int> m_suppressedList = {};
		bool eventFilter(QObject *target, QEvent *event)
		{
			if (event->type() == QKeyEvent::KeyPress)
			{
				event->ignore();
				return true;
			}
			return QObject::eventFilter(target, event);
		}

	private slots:
		void showContextMenu(const QPoint & pos) {
			QMenu contextMenu("Keypoint Menu", this);
			QAction *removeAction = new QAction("Remove Keypoint", this);
			QAction *suppressToggleAction = new QAction(this);
			bool isSuppressed = m_suppressedList.contains(this->currentRow());
			if (isSuppressed) {
				suppressToggleAction->setText("Unsuppress Keypoint");
			}
			else {
				suppressToggleAction->setText("Suppress Keypoint");
			}
			contextMenu.addAction(removeAction);
			contextMenu.addAction(suppressToggleAction);
			QAction *clickedAction = contextMenu.exec(mapToGlobal(pos));
			if (clickedAction == removeAction) {
				emit removeKeypoint(this->currentRow());
			}
			else if (clickedAction == suppressToggleAction) {
				if (isSuppressed) {
					m_suppressedList.removeAll(this->currentRow());
					emit unsuppressKeypoint(this->currentRow());
				}
				else {
					m_suppressedList.append(this->currentRow());
					emit suppressKeypoint(this->currentRow());
				}
			}
		}

		void itemDoubleClickedSlot(QListWidgetItem* item_useless) {
			bool isSuppressed = m_suppressedList.contains(this->currentRow());
			for (const auto &item : this->findItems("", Qt::MatchContains)) {
				if (isSuppressed) {
					m_suppressedList.removeAll(this->row(item));
					emit unsuppressKeypoint(this->row(item));
				}
				else {
					m_suppressedList.append(this->row(item));
					emit suppressKeypoint(this->row(item));
				}
			}
		}
};


class KeypointWidget : public QWidget {
	Q_OBJECT
	public:
		explicit KeypointWidget(QWidget *parent = nullptr);
		void init();
		void toggleHideAll();

	signals:
		void updateViewer();
		void currentEntityChanged(const QString& entity);
		void currentBodypartChanged(const QString& bodypart, QColor color);
		void toggleEntityVisible(const QString& entity, bool toggle);
		void keypointRemoved(const QString& keypointName);
		void keypointSuppressed(const QString& keypointName);
		void keypointUnsuppressed(const QString& keypointName);

	public slots:
		void keypointAddedSlot(Keypoint *keypoint);
		void keypointRemovedSlot(Keypoint *keypoint);
		void keypointCorrectedSlot(Keypoint *keypoint);
		void alreadyAnnotatedSlot(bool isSuppressed);
		void datasetLoadedSlot();
		void removeKeypointSlot(int row);
		void suppressKeypointSlot(int row);
		void unsuppressKeypointSlot(int row);
		void frameChangedSlot(int currentImgSetIndex, int currentFrameIndex);
		void setKeypointsFromDatasetSlot();
		void toggleCurrentKeypointSlot();
		void afterToggleSuppressionSlot();
		void skipCurrentKeypointSlot();

	private:
		ColorMap *colorMap;

		bool m_init = false;

		QGridLayout *keypointlayout;
		QWidget *hideEntityWidget;
		QGridLayout *hideentitylayout;
		QList<QCheckBox*> hideEntitiesBoxesList;
		QList<QString> entitiesList;

		QTabWidget *keypointTabWidget;
		QMap<QString, KeypointListWidget*> keypointListMap;
		QList<QString> bodypartsList;

		QString m_currentEntity;
		QString m_currentBodypart;

		ImgSet *m_currentImgSet;
		int m_currentFrameIndex;
		bool m_allHidden = false;

		bool eventFilter(QObject *target, QEvent *event)
		{
			if (event->type() == QKeyEvent::KeyPress)
			{
				event->ignore();
				return true;
			}
			return QObject::eventFilter(target, event);
		}

	private slots:
		void hideEntitySlot(int toggle);
		void currentTabChangedSlot(int index);
		void itemChangedSlot(QListWidgetItem *item);

};

#endif
