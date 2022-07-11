/*******************************************************************************
 * File:		  viewport.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include <math.h>

#include "viewport.hpp"
#include "dataset.hpp"

#include <QGridLayout>
#include <QFileDialog>
#include<QTimer>

ViewPort::ViewPort(QWidget *parent) : QWidget(parent)
{	
	QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0,0,0,0);
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
	view->defaultFrameGraph()->setClearColor(QColor(34, 36, 40));
    rootEntity = new Qt3DCore::QEntity();
	animalEntity = new Qt3DCore::QEntity(rootEntity);
	camerasEntity = new Qt3DCore::QEntity(rootEntity);
	setupEntity = new Qt3DCore::QEntity(rootEntity);


    // Camera
	camera = view->camera();
	connect(camera, &Qt3DRender::QCamera::upVectorChanged, this, &ViewPort::cameraMoved);
	connect(camera, &Qt3DRender::QCamera::viewCenterChanged, this, &ViewPort::cameraMoved);

	BetterCameraController *camController = new BetterCameraController(rootEntity);
	camController->setLinearSpeed(1000.0f);
	camController->setLookSpeed(360.0f);
	camController->setCamera(camera);


	view->setRootEntity(rootEntity);

	QWidget *container = QWidget::createWindowContainer(view);
    layout->addWidget(container);

}

void ViewPort::adjustView() {
	if (!m_viewAdjust) return; 
	if (!m_camerasVisible && !m_setupVisible) {
		camera->viewEntity(animalEntity);
	}
	else if (m_setupVisible && !m_camerasVisible) {
		if (m_floor == nullptr) 
		{
			camera->viewEntity(animalEntity);
		}
		else {
			camera->viewEntity(setupEntity);
		}
	}
	else {
		camera->viewAll();
	}
}

void ViewPort::setCameraPosition(QVector3D position, QVector3D upVector, QVector3D center) {
    camera->setPosition(position);
	camera->setUpVector(upVector);
	camera->setViewCenter(center);
	adjustView();
}

void ViewPort::setCameraLense(float focalLength, QSize imgSize) {
	camera->lens()->setPerspectiveProjection(360.0/M_PI*atan(imgSize.width()/(2*focalLength)), imgSize.width() / imgSize.height(), 0.1f, 20000.0f);
}

void ViewPort::reset() {
    for (auto & entity : m_keypointEntities.values()) {
        entity->deleteLater();
	}
	m_keypointEntities.clear();
	for (auto & entity : m_skeletonEntities.values()) {
		entity->deleteLater();
	}
	m_skeletonEntities.clear();
	m_skeletonEndPoints.clear();
	for (auto & entity : m_setupKeypoints.values()) {
		entity->deleteLater();
	}
	m_setupKeypoints.clear();
	if (m_floor != nullptr) {
		m_floor->deleteLater();
		m_floor = nullptr;
	}

	for (auto & entity : m_cameraList) {
		entity->deleteLater();
	}
	m_cameraList.clear();
}

Qt3DCore::QEntity* ViewPort::addSphere(QVector3D coords, float radius, QColor color, Qt3DCore::QEntity * parentEntity)
{
	if (parentEntity == nullptr) parentEntity = rootEntity;
	Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(parentEntity);
	material->setAmbient(color);
	Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(parentEntity);
	Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
	sphereMesh->setRadius(radius);

	Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
	sphereTransform->setTranslation(coords);

	sphereEntity->addComponent(sphereMesh);
	sphereEntity->addComponent(sphereTransform);
	sphereEntity->addComponent(material);

	return sphereEntity;
}

Qt3DCore::QEntity* ViewPort::addJoint(QVector3D coordsA, QVector3D coordsB, float radius ,QColor color, float alpha, Qt3DCore::QEntity * parentEntity)
{
	if (parentEntity == nullptr) parentEntity = rootEntity;
	QVector3D vecAB = coordsB - coordsA;
	float vecABLength = vecAB.length();
	Qt3DExtras::QPhongAlphaMaterial *material = new Qt3DExtras::QPhongAlphaMaterial(parentEntity);
	material->setAmbient(color);
	material->setAlpha(alpha);
	Qt3DCore::QEntity *cylinderEntity = new Qt3DCore::QEntity(parentEntity);
	Qt3DExtras::QCylinderMesh *cylinderMesh = new Qt3DExtras::QCylinderMesh;
	cylinderMesh->setRadius(radius);
	cylinderMesh->setLength(vecABLength);
	cylinderMesh->setSlices(100);

	Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;

	QVector3D cylinderVect(0, 1, 0);
	QVector3D a = QVector3D::crossProduct(cylinderVect, vecAB);
	float w = vecABLength + QVector3D::dotProduct(cylinderVect, vecAB);
	QQuaternion q(w,a);
	q.normalize();

	transform->setTranslation(coordsA + q*QVector3D(0, vecABLength / 2, 0));
	transform->setRotation(q);

	cylinderEntity->addComponent(cylinderMesh);
	cylinderEntity->addComponent(transform);
	cylinderEntity->addComponent(material);

	return cylinderEntity;
}

void ViewPort::addCamera(QVector3D position, QVector3D pointingVector, QVector3D upVector) {
	Qt3DCore::QEntity *cameraEntity = new Qt3DCore::QEntity(camerasEntity);
	Qt3DExtras::QPhongAlphaMaterial *material = new Qt3DExtras::QPhongAlphaMaterial(camerasEntity);
	material->setAmbient(QColor(0,255,255));
	Qt3DRender::QMesh *cameraMesh = new Qt3DRender::QMesh();
	cameraMesh->setSource(QUrl::fromLocalFile("/home/timo/Downloads/Camera.obj"));

	Qt3DCore::QTransform *cameraTransform = new Qt3DCore::QTransform;
	QVector3D cameraVect(0, -1, 0);
	cameraVect.normalize();
	QVector3D a = QVector3D::crossProduct(cameraVect, pointingVector);
	float w = pointingVector.length() + QVector3D::dotProduct(cameraVect, pointingVector);
	QQuaternion q(w,a);
	q.normalize();
	
	cameraTransform->setTranslation(position-50*pointingVector);
	cameraTransform->setRotation(q);

	cameraEntity->addComponent(cameraMesh);
	cameraEntity->addComponent(material);
	cameraEntity->addComponent(cameraTransform);
    m_cameraList.append(cameraEntity);

	QTimer::singleShot(50, this, SLOT(toggleCamerasInitSlot()));
}

void ViewPort::addSetup(QList<QVector3D> coords, float cornerRadius, QColor cornerColor, QColor floorColor, float alpha, bool addFloor) {
	for (auto & entity : m_setupKeypoints.values()) {
		entity->deleteLater();
	}
	m_setupKeypoints.clear();
	if (m_floor != nullptr) {
		m_floor->deleteLater();
		m_floor = nullptr;
	}
	if (addFloor) {
		cv::Mat m = cv::Mat(int(coords.size()), 3, CV_64F, 0.0);
		for (int i = 0; i < coords.size(); i++) {
			m.at<double>(i, 0) = coords[i][0];
			m.at<double>(i, 1) = coords[i][1];
			m.at<double>(i, 2) = coords[i][2];
		}
		cv::Vec3d centroid(cv::mean(m.col(0))[0], cv::mean(m.col(1))[0], cv::mean(m.col(2))[0]);

		m.col(0) -= centroid[0];
		m.col(1) -= centroid[1];
		m.col(2) -= centroid[2];

		cv::SVD svd(m.t());

		cv::Vec3d normal =  svd.u.col(svd.u.size().width-1);

		QVector3D qCentroid(centroid[0], centroid[1], centroid[2]);
		QVector3D qNormal(normal[0], normal[1], normal[2]);

		float radius = 0;
		for (int i = 0; i < coords.size(); i++) {
			radius = std::max(radius, qCentroid.distanceToPoint(coords[i]));
		}


		m_floor = addJoint(qCentroid-qNormal*0.1, qCentroid+qNormal*0.2, radius*1.1, floorColor, alpha,setupEntity);
	}
	int i = 0;
	for (const auto & coord : coords) {
		m_setupKeypoints[QString::number(i)] = addSphere(coord, cornerRadius, cornerColor, setupEntity);
		i++;
	}

	QTimer::singleShot(50, this, SLOT(toggleSetupInitSlot()));
}

void ViewPort::update() {
	int count = 0;
	m_center = QVector3D(0,0,0);
	for (const auto &entity : Dataset::dataset->entitiesList())
	{
		for (const auto &bodypart : Dataset::dataset->bodypartsList())
		{
			QString id = entity + "/" + bodypart;
			if (m_coords3D.contains(id)) {
				QVector3D coord3D = m_coords3D[id];
				m_center += coord3D;
				count++;
				if (m_keypointEntities.contains(id)) {
					Qt3DCore::QEntity *keypoint = m_keypointEntities[id];
					Qt3DCore::QTransform *transform = keypoint->componentsOfType<Qt3DCore::QTransform>()[0];
					QVector3D old_translation = transform->translation();
					if (old_translation != coord3D) {
						transform->setTranslation(coord3D);
						adjustView();
						m_camMoveCounter -= 3;
					}
				}
				else {
					QColor color = Dataset::dataset->imgSets()[0]->frames[0]->keypointMap[id]->color();
					m_keypointEntities[id] =  addSphere(coord3D, 5,color, animalEntity);
					adjustView();
					m_camMoveCounter -= 3;
				}
			}
			else {
				if (m_keypointEntities.contains(id)) {
					m_keypointEntities[id]->deleteLater();
					m_keypointEntities.remove(id);
					adjustView();
					m_camMoveCounter -= 3;
				}
			}
		for (const auto &bone : Dataset::dataset->skeleton()) {
			QString idA = entity + "/" + bone.keypointA;
			QString idB = entity + "/" + bone.keypointB;
			QString id = idA + "-" + idB;
			if (m_coords3D.contains(idA) && m_coords3D.contains(idB)) {
				QVector3D coordsA = m_coords3D[idA];
				QVector3D coordsB = m_coords3D[idB];
				if (m_skeletonEntities.contains(id)) {
					QVector3D old_start = m_skeletonEndPoints[id].first;
					QVector3D old_end = m_skeletonEndPoints[id].second;
					if (old_start != coordsA || old_end != coordsB) {
						Qt3DCore::QEntity *bone = m_skeletonEntities[id];
						Qt3DCore::QTransform *transform = bone->componentsOfType<Qt3DCore::QTransform>()[0];
						Qt3DExtras::QCylinderMesh *cylinderMesh = bone->componentsOfType<Qt3DExtras::QCylinderMesh>()[0];
						QVector3D vecAB = coordsB - coordsA;
						float vecABLength = vecAB.length();
						cylinderMesh->setLength(vecABLength);
						QVector3D cylinderVect(0, 1, 0);
						QVector3D a = QVector3D::crossProduct(cylinderVect, vecAB);
						float w = vecABLength + QVector3D::dotProduct(cylinderVect, vecAB);
						QQuaternion q(w,a);
						q.normalize();
						transform->setTranslation(coordsA + q*QVector3D(0, vecABLength / 2, 0));
						transform->setRotation(q);
						adjustView();
						m_camMoveCounter -= 3;
						m_skeletonEndPoints[id] = QPair(coordsA, coordsB);

					}
				}
				else {
					m_skeletonEntities[id] = addJoint(coordsA, coordsB, 1, QColor(150, 150, 150));
					m_skeletonEndPoints[id] = QPair(coordsA, coordsB);
					adjustView();
					m_camMoveCounter -= 3;
				}
			}
			else {
				if (m_skeletonEntities.contains(id)) {
					m_skeletonEntities[id]->deleteLater();
					m_skeletonEntities.remove(id);
					m_skeletonEndPoints.remove(id);
					adjustView();
					m_camMoveCounter -= 3;
				}
			}
		}
		}
	}
	m_center = m_center / count;
}

void ViewPort::toggleCamerasSlot(bool toggle) {
	for (const auto & camera : m_cameraList) {
		camera->setEnabled(toggle);
	}
	m_camerasVisible = toggle;
	adjustView();
}

void ViewPort::toggleCamerasInitSlot() {
	for (const auto & camera : m_cameraList) {
		camera->setEnabled(m_camerasVisible);
	}
}

void ViewPort::toggleSetupSlot(bool toggle) {
	if (m_floor != nullptr) m_floor->setEnabled(toggle);
	for (const auto & point : m_setupKeypoints.values()) {
		point->setEnabled(toggle);
	}
	m_setupVisible = toggle;
	adjustView();
}

void ViewPort::toggleSetupInitSlot() {
	if (m_floor != nullptr) m_floor->setEnabled(m_setupVisible);
	for (const auto & point : m_setupKeypoints.values()) {
		point->setEnabled(m_setupVisible);
	}
	adjustView();
}

void ViewPort::toggleViewAdjustSlot(bool toggle) {
	m_viewAdjust = toggle;
}