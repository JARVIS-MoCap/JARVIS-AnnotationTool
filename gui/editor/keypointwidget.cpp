/*------------------------------------------------------------
 *  keypointwidget.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "keypointwidget.hpp"

#include <QErrorMessage>

KeypointWidget::KeypointWidget(QWidget *parent) : QWidget(parent) {
	colorMap = new ColorMap(ColorMap::Jet);
	QGridLayout *keypointlayout = new QGridLayout(this);
	keypointlayout->setMargin(0);
	keypointlayout->setSpacing(20);

	hideEntityWidget = new QWidget(this);
	hideentitylayout = new QGridLayout(hideEntityWidget);
	hideentitylayout->setMargin(0);
	QLabel *hideEntityLabel = new QLabel("Hide/Show Bodyparts");
	hideEntityLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	hideentitylayout->addWidget(hideEntityLabel,0,0);

	keypointTabWidget = new QTabWidget(this);
	keypointTabWidget->setStyleSheet("QTabBar::tab{padding:4px 6px;"
  																	"background-color:palette(base);"
  																	"border-bottom:3px solid palette(alternate-base);}"
																	"QTabBar::tab:selected,QTabBar::tab:hover{"
  																"background-color: palette(alternate-base);}");

	keypointlayout->addWidget(hideEntityWidget,0,0);
	keypointlayout->addWidget(keypointTabWidget,1,0);


	//--- SIGNAL-SLOT Connections ---//
	//-> Incoming Signals

	//<- Outgoing Signals

	//<-> Relayed Signals
}

void KeypointWidget::init() {
	entitiesList.clear();
	hideEntitiesBoxesList.clear();
	keypointTabWidget->blockSignals(true);
	keypointTabWidget->clear();
	keypointTabWidget->blockSignals(false);

	int i = 1;
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		QCheckBox *hideBox = new QCheckBox(entity,hideEntityWidget);
		hideBox->setChecked(true);
		hideentitylayout->addWidget(hideBox,i++,0);
		hideEntitiesBoxesList.append(hideBox);
		entitiesList.append(hideBox->text());
		connect(hideBox, &QCheckBox::stateChanged, this, &KeypointWidget::hideEntitySlot);
	}
	for (const auto& entity : Dataset::dataset->entitiesList()) {
		KeypointListWidget *bodyPartsListWidget = new KeypointListWidget();
		bodyPartsListWidget->setAlternatingRowColors(true);
		connect(bodyPartsListWidget, &QListWidget::itemClicked, this, &KeypointWidget::itemChangedSlot);
		connect(bodyPartsListWidget, &KeypointListWidget::removeKeypoint, this, &KeypointWidget::removeKeypointSlot);
		connect(bodyPartsListWidget, &KeypointListWidget::suppressKeypoint, this, &KeypointWidget::suppressKeypointSlot);
		connect(bodyPartsListWidget, &KeypointListWidget::unsuppressKeypoint, this, &KeypointWidget::unsuppressKeypointSlot);

		for (const auto& bp : Dataset::dataset->bodypartsList()) {
			QListWidgetItem * bpItem = new QListWidgetItem();
			bpItem->setSizeHint(QSize (100, 35));
			bpItem->setText(bp);
			bpItem->setIcon(QIcon::fromTheme("no_check"));
			bodyPartsListWidget->addItem(bpItem);
		}
		m_currentBodypart = bodyPartsListWidget->item(0)->text();
		bodyPartsListWidget->setCurrentItem(bodyPartsListWidget->item(0));
		keypointTabWidget->addTab(bodyPartsListWidget, entity);
		keypointListMap[entity] = bodyPartsListWidget;
	}
	m_currentEntity = entitiesList[0];
	connect(keypointTabWidget, &QTabWidget::currentChanged, this, &KeypointWidget::currentTabChangedSlot);
	QColor color = colorMap->getColor(0, keypointListMap[m_currentEntity]->count());
	emit currentEntityChanged(m_currentEntity);
	emit currentBodypartChanged(m_currentBodypart, color);
}

void KeypointWidget::keypointAddedSlot(Keypoint *keypoint) {
	if (keypoint->state() == Suppressed) return;
	//m_currentImgSet->frames[m_currentFrameIndex]->keypoints.append(keypoint);
	//m_currentImgSet->frames[m_currentFrameIndex]->keypointMap[keypoint->ID()] = keypoint;
	QListWidget *keypointList = keypointListMap[m_currentEntity];
	keypointList->item(keypointList->currentRow())->setIcon(QIcon::fromTheme("check_blue"));
	if (keypointList->currentRow() < keypointList->count()-1) {
		keypointList->setCurrentRow(keypointList->currentRow()+1);
		QColor color = colorMap->getColor(keypointList->currentRow(), keypointList->count());
		m_currentBodypart = keypointList->item(keypointList->currentRow())->text();
		emit currentBodypartChanged(m_currentBodypart, color);
	}
	else {
		int entityIndex = entitiesList.indexOf(m_currentEntity);
		if (entityIndex < entitiesList.size()-1) {
			m_currentEntity = entitiesList[entityIndex+1];
			keypointList = keypointListMap[m_currentEntity];
			keypointTabWidget->setCurrentIndex(entityIndex+1);
			emit currentEntityChanged(m_currentEntity);
			QColor color = colorMap->getColor(keypointList->currentRow(), keypointList->count());
			m_currentBodypart = keypointList->item(keypointList->currentRow())->text();
			emit currentBodypartChanged(m_currentBodypart, color);
		}
	}
}

void KeypointWidget::keypointRemovedSlot(Keypoint *keypoint) {
	m_currentEntity = keypoint->entity();
	m_currentBodypart = keypoint->bodypart();
	keypointTabWidget->setCurrentIndex(entitiesList.indexOf(m_currentEntity));
	QListWidget* keypointList = keypointListMap[m_currentEntity];
	keypointList->setCurrentRow(Dataset::dataset->bodypartsList().indexOf(m_currentBodypart));
	keypointList->item(keypointList->currentRow())->setIcon(QIcon::fromTheme("no_check"));
	emit currentEntityChanged(m_currentEntity);
	QColor color = colorMap->getColor(keypointList->currentRow(), keypointList->count());
	emit currentBodypartChanged(m_currentBodypart, color);
}

void KeypointWidget::keypointCorrectedSlot(Keypoint *keypoint) {
	QListWidget* keypointList = keypointListMap[keypoint->entity()];
	keypointList->item(Dataset::dataset->bodypartsList().indexOf(keypoint->bodypart()))->setIcon(QIcon::fromTheme("check_blue"));
}


void KeypointWidget::removeKeypointSlot(int row) {
	QListWidget* keypointList = keypointListMap[m_currentEntity];
	keypointList->item(row)->setIcon(QIcon::fromTheme("no_check"));
	QColor color = colorMap->getColor(row, keypointList->count());
	m_currentBodypart = Dataset::dataset->bodypartsList()[row];
	Keypoint *keypoint = m_currentImgSet->frames[m_currentFrameIndex]->keypointMap[m_currentEntity + "/" + m_currentBodypart];
	keypoint->setState(NotAnnotated);
	emit currentBodypartChanged(m_currentBodypart, color);
	emit keypointRemoved(m_currentBodypart);
}

void KeypointWidget::suppressKeypointSlot(int row) {
	QListWidget* keypointList = keypointListMap[m_currentEntity];
	keypointList->item(row)->setIcon(QIcon::fromTheme("discard"));
	QColor color = colorMap->getColor(row, keypointList->count());
	m_currentBodypart = Dataset::dataset->bodypartsList()[row];
	m_currentImgSet->frames[m_currentFrameIndex]->keypointMap[m_currentEntity + "/" + m_currentBodypart]->setState(Suppressed);
	emit currentBodypartChanged(m_currentBodypart, color);
	emit updateViewer();
}

void KeypointWidget::unsuppressKeypointSlot(int row) {
	QListWidget* keypointList = keypointListMap[m_currentEntity];
	keypointList->item(row)->setIcon(QIcon::fromTheme("no_check"));
	QColor color = colorMap->getColor(row, keypointList->count());
	m_currentBodypart = Dataset::dataset->bodypartsList()[row];
	m_currentImgSet->frames[m_currentFrameIndex]->keypointMap[m_currentEntity + "/" + m_currentBodypart]->setState(NotAnnotated);
	emit currentBodypartChanged(m_currentBodypart, color);
	emit keypointUnsuppressed(m_currentBodypart);
}

void KeypointWidget::alreadyAnnotatedSlot(bool isSuppressed) {
    QErrorMessage *msg = new QErrorMessage();
	if (isSuppressed) {
        msg->showMessage("Keypoint is suppressed!");

	}
	else {
        msg->showMessage("Keypoint already annotated!");
	}
}


void KeypointWidget::hideEntitySlot(int toggle) {
	if (toggle == 2) {
		emit toggleEntityVisible(qobject_cast<QCheckBox*>(QObject::sender())->text(),true);
	}
	else {
		emit toggleEntityVisible(qobject_cast<QCheckBox*>(QObject::sender())->text(),false);
	}
}

void KeypointWidget::currentTabChangedSlot(int index) {
	m_currentEntity = entitiesList[index];
	m_currentBodypart = Dataset::dataset->bodypartsList()[keypointListMap[m_currentEntity]->currentRow()];
	emit currentEntityChanged(m_currentEntity);
	QColor color = colorMap->getColor(keypointListMap[m_currentEntity]->currentRow(), keypointListMap[m_currentEntity]->count());
	emit currentBodypartChanged(m_currentBodypart, color);
}

void KeypointWidget::itemChangedSlot(QListWidgetItem *item) {
	QListWidget *keypointList = qobject_cast<QListWidget*>(QObject::sender());
	m_currentBodypart = item->text();
	int index = keypointList->row(item);
	QColor color = colorMap->getColor(index, keypointList->count());
	emit currentBodypartChanged(m_currentBodypart, color);
}

void KeypointWidget::datasetLoadedSlot() {
	m_currentImgSet = Dataset::dataset->imgSets()[0];
	m_currentFrameIndex = 0;
	setKeypointsFromDatasetSlot();
}

void KeypointWidget::setKeypointsFromDatasetSlot() {
	for (const auto& list : keypointListMap) {
		for (int i = 0; i < Dataset::dataset->bodypartsList().size(); i++) {
			list->item(i)->setIcon(QIcon::fromTheme("no_check"));
		}
	}
	for (const auto& keypoint : m_currentImgSet->frames[m_currentFrameIndex]->keypoints) {
		KeypointListWidget* keypointList = qobject_cast<KeypointListWidget*>(keypointListMap[keypoint->entity()]);
		keypointList->setCurrentRow(Dataset::dataset->bodypartsList().indexOf(m_currentBodypart));
		if(keypoint->state() == Reprojected) {
			keypointList->item(Dataset::dataset->bodypartsList().indexOf(keypoint->bodypart()))->setIcon(QIcon::fromTheme("check_small"));
		}
		else if (keypoint->state() == Annotated) {
			keypointList->item(Dataset::dataset->bodypartsList().indexOf(keypoint->bodypart()))->setIcon(QIcon::fromTheme("check_blue"));
		}
		else if (keypoint->state() == Suppressed){
			keypointList->addSupressed(Dataset::dataset->bodypartsList().indexOf(keypoint->bodypart()));
			keypointList->item(Dataset::dataset->bodypartsList().indexOf(keypoint->bodypart()))->setIcon(QIcon::fromTheme("discard"));
		}
	}
}

void KeypointWidget::frameChangedSlot(int currentImgSetIndex, int currentFrameIndex) {
	Dataset::dataset->save();
	std::cout << "ImgSet: " << currentImgSetIndex << ", Frame: " << currentFrameIndex << std::endl;
	m_currentImgSet = Dataset::dataset->imgSets()[currentImgSetIndex];
	m_currentFrameIndex = currentFrameIndex;
	setKeypointsFromDatasetSlot();
}
