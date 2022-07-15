/*******************************************************************************
 * File:			  viewport.hpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "globals.hpp"
#include "colormap.hpp"
#include "reprojectiontool.hpp"

#include <opencv2/core.hpp>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QMesh>
//#include <Qt3DCore/QAspectEngine>
//#include <Qt3DInput/QInputAspect>

//#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QCylinderMesh>

#include <Qt3DExtras/QOrbitCameraController>
#include "bettercameracontroller.hpp"



class ViewPort : public QWidget
{Q_OBJECT
	public:
		explicit ViewPort(QWidget *parent = nullptr);

        void reset();
        void setCoords3D(QMap<QString, QVector3D> coords3D) {m_coords3D = coords3D;}
		QMap<QString, QVector3D> coords3D() {return m_coords3D;}
		QVector3D center() {return m_center;}
		void addCamera(QVector3D position, QVector3D pointingVector, QVector3D upVector);
		void addSetup(QList<QVector3D> coords, float cornerRadius, QColor cornerColor, QColor floorColor, float alpha = 1.0, bool addFloor = true);
		void setCameraLense(float focalLength, QSize imgSize);

	public slots:
        void setCameraPosition(QVector3D position, QVector3D upVector, QVector3D center);

	signals:
    	void cameraMoved();


	public slots:
		void update();
		void toggleCamerasSlot(bool toggle);
		void toggleCamerasInitSlot();
		void toggleSetupSlot(bool toggle);
		void toggleSetupInitSlot();
		void toggleViewAdjustSlot(bool toggle);
		void setKeypointRadiusSlot(int radius);
		void setSkeletonThicknessSlot(int thickness);



	private:
		Qt3DCore::QEntity *rootEntity;
		Qt3DCore::QEntity *animalEntity;
		Qt3DCore::QEntity *camerasEntity;
		Qt3DCore::QEntity *setupEntity;


		QMap<QString, QVector3D> m_coords3D;
		QVector3D m_center;
		
		bool m_setupVisible = true;
		bool m_camerasVisible = true;
		bool m_viewAdjust = true;

		int m_keypointRadius = 5;
		int m_skeletonThickness = 1;


		Qt3DRender::QCamera *camera;

		int m_camMoveCounter = 0;
		QMap<QString, Qt3DCore::QEntity*> m_keypointEntities;
		QMap<QString, Qt3DCore::QEntity *> m_skeletonEntities;
		QMap<QString, QPair<QVector3D, QVector3D>> m_skeletonEndPoints;
		QMap<QString, Qt3DCore::QEntity*> m_setupKeypoints;
		Qt3DCore::QEntity* m_floor = nullptr;
		QList<Qt3DCore::QEntity*> m_cameraList;

		Qt3DCore::QEntity* addSphere(QVector3D coords, float radius, QColor color, Qt3DCore::QEntity * parentEntity = nullptr);
		Qt3DCore::QEntity* addJoint(QVector3D coordsA, QVector3D coordsB, float radius, QColor color,float alpha = 1.0, Qt3DCore::QEntity *parentEntity = nullptr);

		void drawCamera(QVector3D position, QVector3D pointingVector, QVector3D upVector);
		void drawCameras();
		void adjustView();

		private slots: 
};

#endif
