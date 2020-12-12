/*------------------------------------------------------------
 *  imageviewer.hpp
 *  Created: 23. October 2020
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "globals.hpp"
#include "keypoint.hpp"
#include "dataset.hpp"
#include "colormap.hpp"


#include <QPainter>


class ImageViewer : public QWidget {
	Q_OBJECT
	public:
		explicit ImageViewer() {
			this->setMinimumSize(150,150);
			setMouseTracking(true);
			m_defaultColormap = new ColorMap(ColorMap::Jet);
		}
		void scale(float s);
		void fitToScreen();
		void setSize(int w, int h) {m_size = QSize(w,h);}
		QSize sizeHint() const override {return m_size;}

	public slots:
		void setFrame(ImgSet *imgSet, int frameIndex);
		void updateViewer();
		void zoomToggledSlot(bool toggle);
		void panToggledSlot(bool toggle);
		void homeClickedSlot();
		void currentEntityChangedSlot(const QString& entity);
		void currentBodypartChangedSlot(const QString& bodypart, QColor color);
		void toggleEntityVisibleSlot(const QString& entity, bool toggle);
		void toggleReprojectionSlot(bool toggle);
		void imageTransformationChangedSlot(int hueFactor, int saturationFactor, int brightnessFactor);
		void keypointSizeChangedSlot(int size);
		void keypointShapeChangedSlot(const QString& entity, KeypointShape shape);
		void colorMapChangedSlot(const QString& entity, ColorMap::ColorMapType type, QColor color);


	signals:
		void zoomFinished();
		void panFinished();
		void keypointAdded(Keypoint *keypoint);
		void keypointRemoved(Keypoint *keypoint);
		void keypointCorrected(Keypoint *keypoint);
		void alreadyAnnotated(bool isSuppressed);
		void draggingPointFinished(int imgSetIndex, int frameIndex);

	private:
		QPointF scaleToImageCoordinates(QPointF rectStart);
		QPointF transformToImageCoordinates(QPointF rectStart);
		void drawInfoBox(QPainter& p, QPointF point, const QString& entity, const QString& bodypart);
		void applyImageTransformations(int hueFactor, int saturationFactor, int brightnessFactor);

		bool m_setImg = false;
		ImgSet *m_currentImgSet;
		int m_currentFrameIndex;
		QList<QString> hiddenEntityList;
		QString m_currentEntity;
		QString m_currentBodypart;
		QColor m_currentColor;
		QImage m_img;
		QImage m_imgOriginal;
		QSize m_size;
		QRectF m_rect;
		QRectF m_crop;
		QPointF m_rectStart;
		QPointF m_delta;
		QPointF m_reference;
		float m_scale = 1.0;
		bool m_zoomActive = false;
		bool m_zoomStarted = false;
		bool m_panActive = false;
		bool m_panStarted = false;
		Keypoint *m_draggedPoint = nullptr;
		QPointF m_dragDelta;
		QPointF m_dragReference;
		QPointF m_previousPosition;
		int m_widthOffset;
		int m_heightOffset;
		int m_hueFactor = 0;
		int m_saturationFactor = 100;
		int m_brightnessFactor = 100;
		QMap<QString, KeypointShape> m_entityToKeypointShapeMap;
		int m_keypointSize = 8;
		ColorMap *m_defaultColormap;
		QMap<QString, ColorMap*> m_entityToColormapMap;

		void paintEvent(QPaintEvent *) override;
		void mousePressEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *);
};

#endif
