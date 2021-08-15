/*****************************************************************
	* File:			  keypointwidget.hpp
	* Created: 	  23. October 2020
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
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


class KeypointListWidget : public QListWidget {
	Q_OBJECT
	public:
		explicit KeypointListWidget() {
			setContextMenuPolicy(Qt::CustomContextMenu);
			connect(this, &KeypointListWidget::customContextMenuRequested, this, &KeypointListWidget::showContextMenu);
		}
		void addSupressed(int row) {
			m_suppressedList.append(row);
		}

	signals:
		void removeKeypoint(int row);
		void suppressKeypoint(int row);
		void unsuppressKeypoint(int row);

	private:
		QList<int> m_suppressedList = {};

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
};


class KeypointWidget : public QWidget {
	Q_OBJECT
	public:
		explicit KeypointWidget(QWidget *parent = nullptr);
		void init();

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

	private:
		ColorMap *colorMap;

		QWidget *hideEntityWidget;
		QGridLayout *hideentitylayout;
		QList<QCheckBox*> hideEntitiesBoxesList;
		QList<QString> entitiesList;

		QTabWidget *keypointTabWidget;
		QMap<QString, QListWidget*> keypointListMap;
		QList<QString> bodypartsList;

		QString m_currentEntity;
		QString m_currentBodypart;

		ImgSet *m_currentImgSet;
		int m_currentFrameIndex;

	private slots:
		void hideEntitySlot(int toggle);
		void currentTabChangedSlot(int index);
		void itemChangedSlot(QListWidgetItem *item);

};

#endif
