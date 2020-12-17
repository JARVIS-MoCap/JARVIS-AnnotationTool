/*------------------------------------------------------------
 *  imageviewer.cpp
 *  Created:  10. July 2018
 *  Author:   Timo Hüser
 *
 *------------------------------------------------------------*/

#include "imageviewer.hpp"

#include <QMouseEvent>


void ImageViewer::fitToScreen() {
	float scaleWidth = static_cast<float>(this->size().width())/m_crop.width();
	float scaleHeight = static_cast<float>(this->size().height())/m_crop.height();
	m_scale = std::min(scaleHeight, scaleWidth);
	m_delta = QPoint(0,0);
	m_widthOffset = (this->size().width()/m_scale-m_crop.width())/2;
	m_heightOffset = (this->size().height()/m_scale-m_crop.height())/2;
}

void ImageViewer::setFrame(ImgSet *imgSet, int frameIndex) {
	m_currentImgSet = imgSet;
	m_currentFrameIndex = frameIndex;
	m_img = QImage(m_currentImgSet->frames[m_currentFrameIndex]->imagePath);
	m_imgOriginal = m_img;
	if (m_hueFactor != 0 || m_saturationFactor != 100 || m_brightnessFactor != 100) {
		applyImageTransformations(m_hueFactor, m_saturationFactor, m_brightnessFactor);
	}
	m_rect = m_img.rect();
	m_crop = m_rect;
	m_rect.translate(-m_rect.center());
	m_setImg = true;
	homeClickedSlot();
}

void ImageViewer::imageTransformationChangedSlot(int hueFactor, int saturationFactor, int brightnessFactor) {
	m_hueFactor = hueFactor;
	m_saturationFactor = saturationFactor;
	m_brightnessFactor = brightnessFactor;
	applyImageTransformations(hueFactor, saturationFactor, brightnessFactor);
	update();
}

void ImageViewer::keypointSizeChangedSlot(int size) {
	m_keypointSize = size;
	update();
}

void ImageViewer::applyImageTransformations(int hueFactor, int saturationFactor, int brightnessFactor) {
	int h,s,v;
	for (int i = 0; i < m_img.width(); i++) {
		for (int j = 0; j < m_img.height(); j++) {
			QColor color = m_imgOriginal.pixelColor(i,j);
			color.getHsv(&h,&s,&v);
			color.setHsv((h+hueFactor)%360,std::min(s*saturationFactor/100,255),std::min(v*brightnessFactor*brightnessFactor/10000,255));
			m_img.setPixelColor(i,j,color);
		}
	}
}



void ImageViewer::paintEvent(QPaintEvent *) {
	QPainter p{this};
	p.translate(rect().center());
	p.scale(m_scale, m_scale);
	m_rect = m_crop;
	m_rect.translate(-m_crop.center());
	p.drawImage(m_rect, m_img, m_crop);
	if(m_zoomStarted) {
		QPointF rectImg = scaleToImageCoordinates(m_rectStart);
		QPointF deltaImg = scaleToImageCoordinates(m_delta);
		p.drawRect(rectImg.rx()-m_crop.center().rx()+m_crop.topLeft().rx()-m_widthOffset,
					rectImg.ry()-m_crop.center().ry()+m_crop.topLeft().ry()-m_heightOffset, deltaImg.rx(),  deltaImg.ry());
	}
	for (auto& pt : m_currentImgSet->frames[m_currentFrameIndex]->keypoints) {
		if (!hiddenEntityList.contains(pt->entity()) && (pt->state() == Annotated || pt->state() == Reprojected)) {
			QPointF point = transformToImageCoordinates(pt->coordinates());
			if (m_crop.contains(pt->coordinates())) {
				QColor ptColor;
				if (m_entityToColormapMap.contains(pt->entity())) {
					ptColor = m_entityToColormapMap[pt->entity()]->getColor(Dataset::dataset->bodypartsList().indexOf(pt->bodypart()), Dataset::dataset->bodypartsList().size());
				}
				else {
					ptColor = m_defaultColormap->getColor(Dataset::dataset->bodypartsList().indexOf(pt->bodypart()), Dataset::dataset->bodypartsList().size());
				}
				if (pt->state() == Reprojected) {
					ptColor.setAlpha(100);
				}
				else if (pt->state() == Annotated) {
					ptColor.setAlpha(255);
				}
				p.setBrush(ptColor);
				p.setPen(QColor(0,0,0,0));
				if(m_entityToKeypointShapeMap.contains(pt->entity())) {
					if (m_entityToKeypointShapeMap[pt->entity()] == Circle) {
						p.drawEllipse(point,m_keypointSize/2.0,m_keypointSize/2.0);
					}
					else if (m_entityToKeypointShapeMap[pt->entity()] == Rectangle) {
						p.drawRect(QRectF(point.x()-m_keypointSize/2.0, point.y()-m_keypointSize/2.0,m_keypointSize,m_keypointSize));
					}
					else if(m_entityToKeypointShapeMap[pt->entity()] == Triangle) {

					}
				}
				else {
					p.drawEllipse(point,m_keypointSize/2.0,m_keypointSize/2.0);
				}
				if (pt->showName()) {
					drawInfoBox(p,point, pt->entity(), pt->bodypart());
				}
				pt->setShowName(false);
			}
		}
	}
}

void ImageViewer::drawInfoBox(QPainter& p, QPointF point, const QString& entity, const QString& bodypart) {
	p.setPen(QColor(255,255,255,0));
	p.setBrush(QColor(34, 36, 40,200));
	QFont pFont = QFont("Sans Serif", 11);
	pFont.setPixelSize(20/(m_scale+0.2));
	p.setFont(pFont);
	QRect rect = QRect(point.rx()+5, point.ry()+5, 130,50);
	p.drawRoundedRect(p.boundingRect(rect, Qt::AlignLeft, bodypart + "\n" + entity).adjusted(-2,-2,2,2), 5, 10, Qt::RelativeSize);
	p.setPen(QColor(255,255,255));
	p.drawText(rect, Qt::AlignLeft, bodypart + "\n" + entity);
}

QPointF ImageViewer::scaleToImageCoordinates(QPointF point) {
	return QPointF(point.rx()/m_scale, point.ry()/m_scale);
}

QPointF ImageViewer::transformToImageCoordinates(QPointF point) {
	return QPointF(point.rx()-m_crop.center().rx(),
				point.ry()-m_crop.center().ry());
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
	if (!m_setImg) return;
	m_widthOffset = (this->size().width()/m_scale-m_crop.width())/2;
	m_heightOffset = (this->size().height()/m_scale-m_crop.height())/2;
	QPointF position = scaleToImageCoordinates(event->pos());
	position = QPointF(m_crop.topLeft().rx()+position.rx()-m_widthOffset,m_crop.topLeft().ry()+position.ry()-m_heightOffset);
	if (event->button() == Qt::LeftButton && m_zoomActive) {
		m_zoomStarted = true;
		m_rectStart = event->pos();
	}
	else if (event->button() == Qt::LeftButton && m_panActive) {
		m_panStarted = true;
		m_reference = event->pos();
	}
	else if (event->button() == Qt::RightButton) {
		if (hiddenEntityList.contains(m_currentEntity)) return;
		Keypoint *keypoint = m_currentImgSet->frames[m_currentFrameIndex]->keypointMap[m_currentEntity + "/" + m_currentBodypart];
		if(keypoint->state() == NotAnnotated) {
			keypoint->setCoordinates(position);
			keypoint->setState(Annotated);
			emit keypointAdded(keypoint);
			emit keypointChangedForReprojection(Dataset::dataset->imgSets().indexOf(m_currentImgSet), m_currentFrameIndex);
		}
		else {
			emit alreadyAnnotated(keypoint->state() == Suppressed);
		}
		update();
	}
	else if (event->button() == Qt::MiddleButton) {
		if (hiddenEntityList.contains(m_currentEntity)) return;
		for (auto& pt : m_currentImgSet->frames[m_currentFrameIndex]->keypoints) {
			if ((position-pt->coordinates()).manhattanLength() < 6) {
				if (pt->state() == Annotated || pt->state() == Reprojected) {
					pt->setState(NotAnnotated);
					emit keypointRemoved(pt);
					emit keypointChangedForReprojection(Dataset::dataset->imgSets().indexOf(m_currentImgSet), m_currentFrameIndex);
					update();
					break;
				}
			}
		}
	}
	else if (event->button() == Qt::LeftButton) {
		if (hiddenEntityList.contains(m_currentEntity)) return;
		for (auto& pt : m_currentImgSet->frames[m_currentFrameIndex]->keypoints) {
			if ((position-pt->coordinates()).manhattanLength() < 6 && (pt->state() == Annotated || pt->state() == Reprojected)) {
				m_draggedPoint = pt;
				pt->setState(Annotated);
				emit keypointCorrected(pt);
				m_dragReference = event->pos();
				pt->setCoordinates(position);
				update();
				break;
			}
		}
	}
}


void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		//fitToScreen(m_pixmap.size().width());
		//update();
	}
}


void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
	if (m_zoomStarted) {
		m_delta = (event->pos() - m_rectStart);
		update();
	}
	else if (m_panStarted) {
		m_delta = (event->pos() - m_reference);
		m_reference = event->pos();
		QPointF deltaImg = scaleToImageCoordinates(m_delta);
		m_crop = QRectF(m_crop.topLeft().rx()-deltaImg.rx(), m_crop.topLeft().ry()-deltaImg.ry(), m_crop.width(), m_crop.height());
		update();
	}
	if (m_draggedPoint == nullptr) {
		QPointF position = scaleToImageCoordinates(event->pos());
		position = QPointF(m_crop.topLeft().rx()+position.rx()-m_widthOffset,m_crop.topLeft().ry()+position.ry()-m_heightOffset);
		for (auto& pt : m_currentImgSet->frames[m_currentFrameIndex]->keypoints) {
			if ((position-pt->coordinates()).manhattanLength() < 6 && (m_previousPosition-pt->coordinates()).manhattanLength() > 6) {
				pt->setShowName(true);
				update();
				break;
			}
			else if ((position-pt->coordinates()).manhattanLength() > 12 && (m_previousPosition-pt->coordinates()).manhattanLength() < 12) {
				pt->setShowName(false);
				update();
				break;
			}
		}
		m_previousPosition = position;
	}
	else {
		m_dragDelta = (event->pos() - m_dragReference);
		m_dragReference = event->pos();
		QPointF deltaImg = scaleToImageCoordinates(m_dragDelta);
		m_draggedPoint->setCoordinates(m_draggedPoint->coordinates()+deltaImg);
		update();
	}
}


void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton && m_zoomStarted) {
		m_zoomStarted = false;
		QPointF rectImg = scaleToImageCoordinates(m_rectStart);
		QPointF deltaImg = scaleToImageCoordinates(m_delta);
		if (deltaImg.rx() < 0) {
			rectImg.rx() = rectImg.rx()+deltaImg.rx();
			deltaImg.rx() *= -1;
		}
		if (deltaImg.ry() < 0) {
			rectImg.ry() = rectImg.ry()+deltaImg.ry();
			deltaImg.ry() *= -1;
		}
		if (deltaImg.rx() < 10 || deltaImg.ry() < 10) return;
		m_crop = QRectF(m_crop.topLeft().rx()+rectImg.rx()-m_widthOffset,m_crop.topLeft().ry()+rectImg.ry()-m_heightOffset,deltaImg.rx(), deltaImg.ry());
		m_delta = QPoint(0,0);
		fitToScreen();
		update();
		emit zoomFinished();
	}
	else if (event->button() == Qt::LeftButton && m_panStarted) {
		m_panStarted = false;
		fitToScreen();
		update();
		emit panFinished();
	}
	else if (event->button() == Qt::LeftButton && m_draggedPoint != nullptr) {
		emit keypointChangedForReprojection(Dataset::dataset->imgSets().indexOf(m_currentImgSet), m_currentFrameIndex);
		m_draggedPoint = nullptr;
	}
}

void ImageViewer::zoomToggledSlot(bool toggle) {
	m_zoomActive = toggle;
	if (toggle) m_panActive = false;
}

void ImageViewer::panToggledSlot(bool toggle) {
	m_panActive = toggle;
	if (toggle) m_zoomActive = false;
}

void ImageViewer::homeClickedSlot() {
	m_zoomActive = false;
	m_panActive = false;
	m_crop = m_img.rect();
	fitToScreen();
	update();
}

void ImageViewer::currentEntityChangedSlot(const QString& entity) {
	m_currentEntity = entity;
}

void ImageViewer::currentBodypartChangedSlot(const QString& bodypart, QColor color) {
	m_currentBodypart = bodypart;
	m_currentColor = color;
}

void ImageViewer::toggleEntityVisibleSlot(const QString& entity, bool toggle) {
	if (!toggle) {
		hiddenEntityList.append(entity);
		update();
	}
	else {
		hiddenEntityList.removeAll(entity);
		update();
	}
}

void ImageViewer::toggleReprojectionSlot(bool toggle) {
	update();
}

void ImageViewer::updateViewer() {
	update();
}

void ImageViewer::keypointShapeChangedSlot(const QString& entity, KeypointShape shape) {
	m_entityToKeypointShapeMap[entity] = shape;
	update();
}


void ImageViewer::colorMapChangedSlot(const QString& entity, ColorMap::ColorMapType type, QColor color) {
	m_entityToColormapMap[entity] = new ColorMap(type, color);
	update();
}
