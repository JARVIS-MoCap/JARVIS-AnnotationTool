/*****************************************************************
	* File:			  datasetlist.cpp
	* Created: 	  12. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2021 Timo Hueser
	* License:    GPL v3.0
	*****************************************************************/

#include "datasetlist.hpp"
#include "segmentselectorwindow.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QDirIterator>
#include <QErrorMessage>
#include "yaml-cpp/yaml.h"


DatasetList::DatasetList(QList<DatasetExportItem> &datasetExportItems,
			QList<QPair<QString,bool>> &entities, QList<QPair<QString,bool>> &keypoints,
			QList<SkeletonComponent> &skeleton, QWidget *parent) :
			QWidget(parent), m_datasetExportItems(datasetExportItems),
			m_entities(entities), m_keypoints(keypoints), m_skeleton(skeleton) {
	m_currentPath = QDir::homePath();
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(3);

	itemSelectorList = new QListWidget(this);
	itemSelectorList->setFont(QFont("Sans Serif", 12));
	connect(itemSelectorList, &QListWidget::currentItemChanged, this, &DatasetList::currentItemChangedSlot);
	connect(itemSelectorList, &QListWidget::itemDoubleClicked, this, &DatasetList::itemSelectedSlot);
	addItemButton = new QPushButton();
	addItemButton->setIcon(QIcon::fromTheme("plus"));
	addItemButton->setMinimumSize(35,35);
	connect(addItemButton, &QPushButton::clicked, this, &DatasetList::addItemSlot);
	deleteItemButton = new QPushButton();
	deleteItemButton->setIcon(QIcon::fromTheme("discard"));
	deleteItemButton->setMinimumSize(35,35);
	connect(deleteItemButton, &QPushButton::clicked, this, &DatasetList::removeItemSlot);
	layout->addWidget(itemSelectorList,0,0,1,2);
	layout->addWidget(addItemButton,1,0);
	layout->addWidget(deleteItemButton,1,1);
}



void DatasetList::itemSelectedSlot(QListWidgetItem *item) {
	int row = itemSelectorList->row(item);
	SegmentSelectorWindow *segmentSelectorWindow = new SegmentSelectorWindow(m_datasetExportItems[row/2].subSets, this);
	segmentSelectorWindow->exec();
	updateCounts(m_datasetExportItems[row/2]);
}


void DatasetList::addItemSlot() {
	QString path = QFileDialog::getOpenFileName(
							this, "Select Dataset YAML File", m_currentPath, "YAML Files (*.yaml)");
	if (path != "") {
		addItem(path);
	}

}

void DatasetList::addItem(const QString &path) {
	QDir dir = QDir(path);
	QString filePath = path;
	if (path.contains(".yaml")) {
		dir = QFileInfo(path).absoluteDir();
		filePath = dir.absolutePath();
	}
	DatasetExportItem exportItem;
	exportItem.name = dir.dirName();
	exportItem.basePath = filePath;
	exportItem.configFilePath = path;
	YAML::Node datasetYaml = YAML::LoadFile(path.toStdString());
	m_skeleton.clear();
	for (const auto& compYaml : datasetYaml["Skeleton"]) {
		SkeletonComponent comp;
		comp.name = QString::fromStdString(compYaml.first.as<std::string>());
		comp.keypointA = QString::fromStdString(compYaml.second["Keypoints"][0].as<std::string>());
		comp.keypointB = QString::fromStdString(compYaml.second["Keypoints"][1].as<std::string>());
		comp.length = compYaml.second["Length"][0].as<float>();
		m_skeleton.append(comp);
	}

	if (analyseDatasetPath(&exportItem, exportItem.basePath)) {
		addListItem(exportItem.name);
		m_datasetExportItems.append(exportItem);
		emit itemsChanged();
		dir.cdUp();
		m_currentPath = dir.path();
	}
}

QList<QString> DatasetList::itemPaths() {
	QList<QString> paths;
	for (const auto &item : m_datasetExportItems) {
		paths.append(item.configFilePath);
	}
	return paths;
}


bool DatasetList::analyseDatasetPath(DatasetExportItem * exportItem, const QString &basePath) {
	bool foundValidData = false;
	QList<QString> subDirPaths;
	if (basePath == exportItem->basePath) {
		subDirPaths = QDir(basePath).entryList(QDir::AllDirs | QDir::NoDotDot);
	}
	else {
		subDirPaths = QDir(basePath).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

	}
	for (const auto &subDirPath : subDirPaths) {
		QList<QString> subSubDirPaths = QDir(basePath + "/" + subDirPath).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
		bool validDir = subSubDirPaths.size() > 0;
		for (const auto &subSubDirPath : subSubDirPaths) {
			if (!QFile::exists(basePath + "/" + subDirPath + "/" + subSubDirPath + "/annotations.csv")) {
				validDir = false;
			}
		}
		if (validDir) {
			QDir baseDir = QDir(exportItem->basePath);
			if (getDatasetDirInfo(exportItem, basePath + "/" + subDirPath)) {
				QPair<QString, bool> subSet;
				subSet.first = baseDir.relativeFilePath(basePath + "/" + subDirPath);
				subSet.second = true;
				exportItem->subSets.append(subSet);
				foundValidData = true;
			}
		}
		else {
			if (subDirPath != "." && analyseDatasetPath(exportItem, basePath + "/" + subDirPath)) {
				foundValidData = true;
			}
		}
	}
	return foundValidData;
}


bool DatasetList::getDatasetDirInfo(DatasetExportItem * exportItem, const QString &path) {
	QList<QString> cameraPaths = QDir(path).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for (const auto &cameraPath : cameraPaths) {
		QFile *file = new QFile(path + "/" + cameraPath + "/annotations.csv");
		if (!file->open(QIODevice::ReadOnly)) {
			return false;
		}
		file->readLine(); //Scorer

		QList<QByteArray> entities = file->readLine().split(',');
		QList<QString> entityNames;
		for (const auto & entity : m_entities) {
			entityNames.append(entity.first);
		}
		//if (m_entities.size() == 0) {
			for (int i = 1; i < entities.size(); i+= 3) {
				if(!entityNames.contains(entities[i])) {
					QPair<QString, bool> newEntity;
					newEntity.first = entities[i];
					newEntity.second = true;
					m_entities.append(newEntity);
					entityNames.append(entities[i]);
				}
			}
	//	}
		// else {
		// 	QList<QString> testEntities;
		// 	for (int i = 1; i < entities.size(); i+= 3) {
 		// 		if(!testEntities.contains(entities[i])) entities.append(entities[i]);
		// 	}
		// 	if (testEntities != entityNames) {
		// 		file->close();
		// 		return false;
		// 	}
		// }

		QList<QByteArray> keypoints = file->readLine().split(',');
		QList<QString> keypointNames;
		for (const auto & keypoint : m_keypoints) {
			keypointNames.append(keypoint.first);
		}
		if (m_keypoints.size() == 0) {
			for (int i = 1; i < keypoints.size(); i+= 3) {
				if(!keypointNames.contains(keypoints[i])) {
					QPair<QString, bool> newKeypoint;
					newKeypoint.first = keypoints[i];
					newKeypoint.second = true;
					m_keypoints.append(newKeypoint);
					keypointNames.append(keypoints[i]);
				}
			}
		}
		else {
			QList<QString> testKeypoints;
			for (int i = 1; i < keypoints.size(); i+= 3) {
				if(!testKeypoints.contains(keypoints[i])) testKeypoints.append(keypoints[i]);
			}
			if (testKeypoints != keypointNames) {
				file->close();
				return false;
			}
		}
		file->readLine(); //Coords
		while (!file->atEnd()) {
			exportItem->frameCount++;
			QList<QByteArray> cells = file->readLine().split(',');
			for (int i = 3; i < cells.size(); i += 3) {
				KeypointState state = static_cast<KeypointState>(cells[i].toInt());
				if (state == Annotated) exportItem->annotationCount.annotated++;
				else if (state == Reprojected) exportItem->annotationCount.reprojected++;
				else if (state == NotAnnotated) exportItem->annotationCount.notAnnotated++;
			}
		}
		file->close();
	}
	return true;
}

void DatasetList::updateCounts(DatasetExportItem &exportItem) {
	exportItem.annotationCount.annotated = 0;
	exportItem.annotationCount.reprojected = 0;
	exportItem.annotationCount.notAnnotated = 0;
	exportItem.frameCount = 0;
	for (const auto &subSet : exportItem.subSets) {
		if (subSet.second) {
			QList<QString> cameraPaths = QDir(exportItem.basePath + "/" + subSet.first).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
			for (const auto &cameraPath : cameraPaths) {
				QFile *file = new QFile(exportItem.basePath + "/" + subSet.first + "/" + cameraPath + "/annotations.csv");
				if (!file->open(QIODevice::ReadOnly)) {
					return;
				}
				file->readLine(); //Scorer
				QList<QByteArray> entities = file->readLine().split(',');
				QList<QString> entityNames;
				for (const auto & entity : m_entities) {
					entityNames.append(entity.first);
				}
				QList<QByteArray> keypoints = file->readLine().split(',');
				QList<QString> keypointNames;
				for (const auto & keypoint : m_keypoints) {
					keypointNames.append(keypoint.first);
				}

				file->readLine(); //Coords
				while (!file->atEnd()) {
					exportItem.frameCount++;
					QList<QByteArray> cells = file->readLine().split(',');
					for (int i = 3; i < cells.size(); i += 3) {
						int entityIndex = entityNames.indexOf(entities[i]);
						int keypointIndex = keypointNames.indexOf(keypoints[i]);
						if (entityIndex != -1 && m_entities[entityIndex].second) {
							if (keypointIndex != -1 && m_keypoints[keypointIndex].second) {
								KeypointState state = static_cast<KeypointState>(cells[i].toInt());
								if (state == Annotated) exportItem.annotationCount.annotated++;
								else if (state == Reprojected) exportItem.annotationCount.reprojected++;
								else if (state == NotAnnotated) exportItem.annotationCount.notAnnotated++;
							}
						}
					}
				}
				file->close();
			}
		}
	}
	emit itemsChanged();
}

void DatasetList::removeItemSlot() {
	int row = itemSelectorList->currentRow();
	if (row != -1) {
		delete itemSelectorList->takeItem(row);
		delete itemSelectorList->takeItem(row);
		m_datasetExportItems.removeAt(row/2);
	}
	if (m_datasetExportItems.size() == 0) {
		m_entities.clear();
		m_keypoints.clear();
	}
	emit itemsChanged();
}


void DatasetList::addListItem(const QString &text) {
	QListWidgetItem * item = new QListWidgetItem();
	item->setSizeHint(QSize (100, 27));
	item->setText(text);
	item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
	itemSelectorList->addItem(item);
	QListWidgetItem * seperatorItem = new QListWidgetItem();
	seperatorItem->setSizeHint(QSize (100, 3));
	seperatorItem->setFlags(Qt::NoItemFlags);
	seperatorItem->setBackground(QColor(46, 50, 60));
	itemSelectorList->addItem(seperatorItem);
}


void DatasetList::currentItemChangedSlot(QListWidgetItem *current,
			QListWidgetItem *previous) {
	if (current != nullptr) {
		current->setBackground(QColor(100,164,32));
	}
	if (previous != nullptr) {
		previous->setBackground(QColor(34, 36, 40));
	}
}

void DatasetList::updateCountsSlot() {
	for (auto &exportItem : m_datasetExportItems) {
		updateCounts(exportItem);
	}
}
