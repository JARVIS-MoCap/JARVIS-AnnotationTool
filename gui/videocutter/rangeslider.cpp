/*=========================================================================
  Library:   CTK
  Copyright (c) Kitware Inc.
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
      http://www.apache.org/licenses/LICENSE-2.0.txt
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
=========================================================================*/

// Qt includes
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QStyleOptionSlider>
#include <QApplication>
#include <QStylePainter>
#include <QStyle>
#include <QToolTip>
#include <QProxyStyle>


// CTK includes
#include "rangeslider.hpp"

class SliderProxy : public QProxyStyle
{
public:
    int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
    {
        switch(metric) {
        case PM_SliderThickness  : return 30;
        case PM_SliderLength     : return 15;
        default                         : return (QProxyStyle::pixelMetric(metric,option,widget));
        }
    }
};


class ctkRangeSliderPrivate
{
  Q_DECLARE_PUBLIC(ctkRangeSlider);
protected:
  ctkRangeSlider* const q_ptr;
public:
  /// Boolean indicates the selected handle
  ///   True for the minimum range handle, false for the maximum range handle
  enum Handle {
    NoHandle = 0x0,
    MinimumHandle = 0x1,
    MaximumHandle = 0x2,
    MainHandle = 0x4
  };
  Q_DECLARE_FLAGS(Handles, Handle);

  ctkRangeSliderPrivate(ctkRangeSlider& object);
  void init();

  /// Return the handle at the given pos, or none if no handle is at the pos.
  /// If a handle is selected, handleRect is set to the handle rect.
  /// otherwise return NoHandle and handleRect is set to the combined rect of
  /// the min and max handles
  Handle handleAtPos(const QPoint& pos, QRect& handleRect)const;

  /// Copied verbatim from QSliderPrivate class (see QSlider.cpp)
  int pixelPosToRangeValue(int pos) const;
  int pixelPosFromRangeValue(int val) const;

  /// Draw the bottom and top sliders.
  void drawMinimumSlider( QStylePainter* painter ) const;
  void drawMaximumSlider( QStylePainter* painter ) const;
  void drawMainSlider( QStylePainter* painter ) const;

  bool m_rangeVisible;
  bool m_mainVisible;
  bool m_mainOverRange;
  bool m_maxInRange;
  bool m_minInRange;


  /// End points of the range on the Model
  int m_MaximumValue;
  int m_MinimumValue;
  int m_MainValue;

  /// End points of the range on the GUI. This is synced with the model.
  int m_MaximumPosition;
  int m_MinimumPosition;
  int m_MainPosition;

  /// Controls selected ?
  QStyle::SubControl m_MinimumSliderSelected;
  QStyle::SubControl m_MaximumSliderSelected;
  QStyle::SubControl m_MainSliderSelected;


  /// See QSliderPrivate::clickOffset.
  /// Overrides this ivar
  int m_SubclassClickOffset;

  /// See QSliderPrivate::position
  /// Overrides this ivar.
  int m_SubclassPosition;

  /// Original width between the 2 bounds before any moves
  int m_SubclassWidth;

  ctkRangeSliderPrivate::Handles m_SelectedHandles;

  /// When symmetricMoves is true, moving a handle will move the other handle
  /// symmetrically, otherwise the handles are independent.
  bool m_SymmetricMoves;

  QString m_HandleToolTip;

private:
  Q_DISABLE_COPY(ctkRangeSliderPrivate);
};

// --------------------------------------------------------------------------
ctkRangeSliderPrivate::ctkRangeSliderPrivate(ctkRangeSlider& object)
  :q_ptr(&object)
{
  this->m_MinimumValue = 25;
  this->m_MaximumValue = 75;
  this->m_MainValue = 0;
  this->m_MinimumPosition = 25;
  this->m_MaximumPosition = 75;
  this->m_MainPosition = 0;
  this->m_MinimumSliderSelected = QStyle::SC_None;
  this->m_MaximumSliderSelected = QStyle::SC_None;
  this->m_MainSliderSelected = QStyle::SC_None;
  this->m_SubclassClickOffset = 0;
  this->m_SubclassPosition = 0;
  this->m_SubclassWidth = 0;
  this->m_SelectedHandles = NoHandle;
  this->m_SymmetricMoves = false;
  this->m_rangeVisible = true;
  this->m_mainVisible = true;
  this->m_mainOverRange = false;
  this->m_maxInRange = true;
  this->m_minInRange = true;

}

// --------------------------------------------------------------------------
void ctkRangeSliderPrivate::init()
{
  Q_Q(ctkRangeSlider);
  this->m_MinimumValue = q->maximum()*0.25;
  this->m_MaximumValue = q->maximum()*0.75;
  this->m_MainValue = q->minimum();
  this->m_MinimumPosition = q->maximum()*0.25;
  this->m_MaximumPosition = q->maximum()*0.75;
  this->m_MainPosition = q->minimum();
  q->connect(q, SIGNAL(rangeChanged(int,int)), q, SLOT(onRangeChanged(int,int)));
}

// --------------------------------------------------------------------------
ctkRangeSliderPrivate::Handle ctkRangeSliderPrivate::handleAtPos(const QPoint& pos, QRect& handleRect)const
{
  Q_Q(const ctkRangeSlider);

  QStyleOptionSlider option;
  q->initStyleOption( &option );

  // The functinos hitTestComplexControl only know about 1 handle. As we have
  // 2, we change the position of the handle and test if the pos correspond to
  // any of the 2 positions.

  // Test the MinimumHandle
  option.sliderPosition = this->m_MinimumPosition;
  option.sliderValue    = this->m_MinimumValue;

  QStyle::SubControl minimumControl = q->style()->hitTestComplexControl(
    QStyle::CC_Slider, &option, pos, q);
  QRect minimumHandleRect = q->style()->subControlRect(
      QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, q);

  // Test if the pos is under the Maximum handle
  option.sliderPosition = this->m_MaximumPosition;
  option.sliderValue    = this->m_MaximumValue;

  QStyle::SubControl maximumControl = q->style()->hitTestComplexControl(
    QStyle::CC_Slider, &option, pos, q);
  QRect maximumHandleRect = q->style()->subControlRect(
      QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, q);

    // Test if the pos is under the Main handle
    option.sliderPosition = this->m_MainPosition;
    option.sliderValue    = this->m_MainValue;

    QStyle::SubControl mainControl = q->style()->hitTestComplexControl(
      QStyle::CC_Slider, &option, pos, q);
    QRect mainHandleRect = q->style()->subControlRect(
        QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, q);

  // The pos is above both handles, select the closest handle
  if (minimumControl == QStyle::SC_SliderHandle &&
      maximumControl == QStyle::SC_SliderHandle)
    {
    int minDist = 0;
    int maxDist = 0;
    if (q->orientation() == Qt::Horizontal)
      {
      minDist = pos.x() - minimumHandleRect.left();
      maxDist = maximumHandleRect.right() - pos.x();
      }
    else //if (q->orientation() == Qt::Vertical)
      {
      minDist = minimumHandleRect.bottom() - pos.y();
      maxDist = pos.y() - maximumHandleRect.top();
      }
    Q_ASSERT( minDist >= 0 && maxDist >= 0);
    minimumControl = minDist < maxDist ? minimumControl : QStyle::SC_None;
    }

  if (mainControl == QStyle::SC_SliderHandle) {
    handleRect = mainHandleRect;
    return MainHandle;
  }

  if (minimumControl == QStyle::SC_SliderHandle)
    {
    handleRect = minimumHandleRect;
    return MinimumHandle;
    }
  else if (maximumControl == QStyle::SC_SliderHandle)
    {
    handleRect = maximumHandleRect;
    return MaximumHandle;
    }
  handleRect = minimumHandleRect.united(maximumHandleRect);
  return NoHandle;
}

// --------------------------------------------------------------------------
// Copied verbatim from QSliderPrivate::pixelPosToRangeValue. See QSlider.cpp
//
int ctkRangeSliderPrivate::pixelPosToRangeValue( int pos ) const
{
  Q_Q(const ctkRangeSlider);
  QStyleOptionSlider option;
  q->initStyleOption( &option );

  QRect gr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderGroove,
                                            q );
  QRect sr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            q );
  int sliderMin, sliderMax, sliderLength;
  if (option.orientation == Qt::Horizontal)
    {
    sliderLength = sr.width();
    sliderMin = gr.x();
    sliderMax = gr.right() - sliderLength + 1;
    }
  else
    {
    sliderLength = sr.height();
    sliderMin = gr.y();
    sliderMax = gr.bottom() - sliderLength + 1;
    }

  return QStyle::sliderValueFromPosition( q->minimum(),
                                          q->maximum(),
                                          pos - sliderMin,
                                          sliderMax - sliderMin,
                                          option.upsideDown );
}

//---------------------------------------------------------------------------
int ctkRangeSliderPrivate::pixelPosFromRangeValue( int val ) const
{
  Q_Q(const ctkRangeSlider);
  QStyleOptionSlider option;
  q->initStyleOption( &option );

  QRect gr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderGroove,
                                            q );
  QRect sr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            q );
  int sliderMin, sliderMax, sliderLength;
  if (option.orientation == Qt::Horizontal)
    {
    sliderLength = sr.width();
    sliderMin = gr.x();
    sliderMax = gr.right() - sliderLength + 1;
    }
  else
    {
    sliderLength = sr.height();
    sliderMin = gr.y();
    sliderMax = gr.bottom() - sliderLength + 1;
    }

  return QStyle::sliderPositionFromValue( q->minimum(),
                                          q->maximum(),
                                          val,
                                          sliderMax - sliderMin,
                                          option.upsideDown ) + sliderMin;
}

//---------------------------------------------------------------------------
// Draw slider at the bottom end of the range
void ctkRangeSliderPrivate::drawMinimumSlider( QStylePainter* painter ) const
{
  Q_Q(const ctkRangeSlider);
  QStyleOptionSlider option;
  q->initMinimumSliderStyleOption( &option );

  option.subControls = QStyle::SC_SliderHandle;
  option.sliderValue = m_MinimumValue;
  option.sliderPosition = m_MinimumPosition;
  if (q->isMinimumSliderDown())
    {
    option.activeSubControls = QStyle::SC_SliderHandle;
    option.state |= QStyle::State_Sunken;
    }
#ifdef Q_OS_MAC
  // On mac style, drawing just the handle actually draws also the groove.
  QRect clip = q->style()->subControlRect(QStyle::CC_Slider, &option,
                                          QStyle::SC_SliderHandle, q);
  painter->setClipRect(clip);
#endif
  painter->drawComplexControl(QStyle::CC_Slider, option);
  painter->drawPixmap(pixelPosFromRangeValue(m_MinimumPosition), 2, 15, 26, QPixmap("/home/timo/Documents/AnnotationTool/bracket.png"));
  //painter->drawRect(m_MinimumPosition, 0, 48, 48);
}

//---------------------------------------------------------------------------
// Draw slider at the top end of the range
void ctkRangeSliderPrivate::drawMaximumSlider( QStylePainter* painter ) const
{
  Q_Q(const ctkRangeSlider);
  QStyleOptionSlider option;
  q->initMaximumSliderStyleOption( &option );

  option.subControls = QStyle::SC_SliderHandle;
  option.sliderValue = m_MaximumValue;
  option.sliderPosition = m_MaximumPosition;
  if (q->isMaximumSliderDown())
    {
    option.activeSubControls = QStyle::SC_SliderHandle;
    option.state |= QStyle::State_Sunken;
    }
#ifdef Q_OS_MAC
  // On mac style, drawing just the handle actually draws also the groove.
  QRect clip = q->style()->subControlRect(QStyle::CC_Slider, &option,
                                          QStyle::SC_SliderHandle, q);
  painter->setClipRect(clip);
#endif
  painter->drawComplexControl(QStyle::CC_Slider, option);
  painter->drawPixmap(pixelPosFromRangeValue(m_MaximumPosition), 2, 15, 26, QPixmap("/home/timo/Documents/AnnotationTool/bracket_max.png"));
}

//---------------------------------------------------------------------------
// Draw slider at the top end of the range
void ctkRangeSliderPrivate::drawMainSlider( QStylePainter* painter ) const
{
  Q_Q(const ctkRangeSlider);
  QStyleOptionSlider option;
  q->initMainSliderStyleOption( &option );

  option.subControls = QStyle::SC_SliderHandle;
  option.sliderValue = m_MainValue;
  option.sliderPosition = m_MainPosition;
  if (q->isMainSliderDown())
    {
    option.activeSubControls = QStyle::SC_SliderHandle;
    option.state |= QStyle::State_Sunken;
    }
#ifdef Q_OS_MAC
  // On mac style, drawing just the handle actually draws also the groove.
  QRect clip = q->style()->subControlRect(QStyle::CC_Slider, &option,
                                          QStyle::SC_SliderHandle, q);
  painter->setClipRect(clip);
#endif
  painter->drawComplexControl(QStyle::CC_Slider, option);
  painter->drawPixmap(pixelPosFromRangeValue(m_MainPosition), 0, 15, 30, QPixmap("/home/timo/Documents/AnnotationTool/mainSlider.png"));
}

// --------------------------------------------------------------------------
ctkRangeSlider::ctkRangeSlider(QWidget* _parent)
  : QSlider(_parent)
  , d_ptr(new ctkRangeSliderPrivate(*this))
{
  Q_D(ctkRangeSlider);
  d->init();
}

// --------------------------------------------------------------------------
ctkRangeSlider::ctkRangeSlider( Qt::Orientation o,
                                  QWidget* parentObject )
  :QSlider(o, parentObject)
  , d_ptr(new ctkRangeSliderPrivate(*this))
{

  Q_D(ctkRangeSlider);
  d->init();
  SliderProxy *aSliderProxy = new SliderProxy();

  setStyleSheet("QSlider::handle::horizontal{image:url(empty);}"
                "QSlider::sub-page::horizontal{background:rgba(255,255,255,0);}");
  setStyle(aSliderProxy);

}

// --------------------------------------------------------------------------
ctkRangeSlider::ctkRangeSlider(ctkRangeSliderPrivate* impl, QWidget* _parent)
  : QSlider(_parent)
  , d_ptr(impl)
{
  Q_D(ctkRangeSlider);
  d->init();
}

// --------------------------------------------------------------------------
ctkRangeSlider::ctkRangeSlider( ctkRangeSliderPrivate* impl, Qt::Orientation o,
                                QWidget* parentObject )
  :QSlider(o, parentObject)
  , d_ptr(impl)
{
  Q_D(ctkRangeSlider);
  d->init();
}

// --------------------------------------------------------------------------
ctkRangeSlider::~ctkRangeSlider()
{
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setRangeVisible(bool visible) {
  Q_D(ctkRangeSlider);
  d->m_rangeVisible = visible;
  this->update();
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMainVisible(bool visible) {
  Q_D(ctkRangeSlider);
  d->m_mainVisible = visible;
  this->update();
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMainOverRange(bool overRange) {
  Q_D(ctkRangeSlider);
  d->m_mainOverRange = overRange;
  this->update();
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMinInRange(bool inRange) {
  Q_D(ctkRangeSlider);
  d->m_minInRange = inRange;
  this->update();
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMaxInRange(bool inRange) {
  Q_D(ctkRangeSlider);
  d->m_maxInRange = inRange;
  this->update();
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::rangeVisible() {
  Q_D(ctkRangeSlider);
  return d->m_rangeVisible;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::mainVisible() {
  Q_D(ctkRangeSlider);
  return d->m_mainVisible;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::mainOverRange() {
  Q_D(ctkRangeSlider);
  return d->m_mainOverRange;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::minInRange() {
  Q_D(ctkRangeSlider);
  return d->m_minInRange;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::maxInRange() {
  Q_D(ctkRangeSlider);
  return d->m_maxInRange;
}

// --------------------------------------------------------------------------
int ctkRangeSlider::minimumValue() const
{
  Q_D(const ctkRangeSlider);
  return d->m_MinimumValue;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMinimumValue( int min )
{
  Q_D(ctkRangeSlider);
  this->setValues( min, qMax(d->m_MaximumValue,min) , d->m_MainValue);
}

// --------------------------------------------------------------------------
int ctkRangeSlider::maximumValue() const
{
  Q_D(const ctkRangeSlider);
  return d->m_MaximumValue;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMaximumValue( int max )
{
  Q_D(ctkRangeSlider);
  this->setValues( qMin(d->m_MinimumValue, max), max , d->m_MainValue);
}

// --------------------------------------------------------------------------
int ctkRangeSlider::mainValue() const
{
  Q_D(const ctkRangeSlider);
  return d->m_MainValue;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMainValue( int main)
{
  Q_D(ctkRangeSlider);
  this->setValues(d->m_MinimumValue, d->m_MaximumValue, main);
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setValues(int min, int max, int main)
{
  Q_D(ctkRangeSlider);
  const int minValue =
    qBound(this->minimum(), qMin(min,max), this->maximum());
  const int maxValue =
    qBound(this->minimum(), qMax(min,max), this->maximum());
  const int mainValue =
    qBound(this->minimum(), main, this->maximum());
  bool emitMinValChanged = (minValue != d->m_MinimumValue);
  bool emitMaxValChanged = (maxValue != d->m_MaximumValue);
  bool emitMainValChanged = (mainValue != d->m_MainValue);


  d->m_MinimumValue = minValue;
  d->m_MaximumValue = maxValue;
  d->m_MainValue = mainValue;

  bool emitMinPosChanged =
    (minValue != d->m_MinimumPosition);
  bool emitMaxPosChanged =
    (maxValue != d->m_MaximumPosition);
  bool emitMainPosChanged =
    (maxValue != d->m_MainPosition);
  d->m_MinimumPosition = minValue;
  d->m_MaximumPosition = maxValue;
  d->m_MainPosition = mainValue;

  if (isSliderDown())
    {
    if (emitMinPosChanged || emitMaxPosChanged || emitMainPosChanged)
      {
      emit positionsChanged(d->m_MinimumPosition, d->m_MaximumPosition, d->m_MainPosition);
      }
    if (emitMinPosChanged)
      {
      emit minimumPositionChanged(d->m_MinimumPosition);
      }
    if (emitMaxPosChanged)
      {
      emit maximumPositionChanged(d->m_MaximumPosition);
      }
    if (emitMainPosChanged)
    {
      emit mainPositionChanged(d->m_MainPosition);
    }
    }
  if (emitMinValChanged || emitMaxValChanged || emitMainValChanged)
    {
    emit valuesChanged(d->m_MinimumValue,
                       d->m_MaximumValue,
                       d->m_MainValue);
    }
  if (emitMinValChanged)
    {
    emit minimumValueChanged(d->m_MinimumValue);
    }
  if (emitMaxValChanged)
    {
    emit maximumValueChanged(d->m_MaximumValue);
    }
  if (emitMainValChanged)
    {
    emit mainValueChanged(d->m_MainValue);
    }
  if (emitMinPosChanged || emitMaxPosChanged ||
      emitMinValChanged || emitMaxValChanged ||
      emitMainPosChanged || emitMainValChanged)
    {
    this->update();
    }
}

// --------------------------------------------------------------------------
int ctkRangeSlider::minimumPosition() const
{
  Q_D(const ctkRangeSlider);
  return d->m_MinimumPosition;
}

// --------------------------------------------------------------------------
int ctkRangeSlider::maximumPosition() const
{
  Q_D(const ctkRangeSlider);
  return d->m_MaximumPosition;
}

// --------------------------------------------------------------------------
int ctkRangeSlider::mainPosition() const
{
  Q_D(const ctkRangeSlider);
  return d->m_MainPosition;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMinimumPosition(int min)
{
  Q_D(const ctkRangeSlider);
  this->setPositions(min, qMax(min, d->m_MaximumPosition), d->m_MainPosition);
}


// --------------------------------------------------------------------------
void ctkRangeSlider::setMaximumPosition(int max)
{
  Q_D(const ctkRangeSlider);
  this->setPositions(qMin(d->m_MinimumPosition, max), max, d->m_MainPosition);
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setMainPosition(int main)
{
  Q_D(const ctkRangeSlider);
  this->setPositions(d->m_MinimumPosition, d->m_MaximumPosition, main);
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setPositions(int min, int max, int main)
{
  Q_D(ctkRangeSlider);
  const int minPosition =
    qBound(this->minimum(), qMin(min, max), this->maximum());
  const int maxPosition =
    qBound(this->minimum(), qMax(min, max), this->maximum());
  const int mainPosition =
    qBound(this->minimum(), main, this->maximum());

  bool emitMinPosChanged = (minPosition != d->m_MinimumPosition);
  bool emitMaxPosChanged = (maxPosition != d->m_MaximumPosition);
  bool emitMainPosChanged = (mainPosition != d->m_MainPosition);


  if (!emitMinPosChanged && !emitMaxPosChanged && !emitMainPosChanged)
    {
    return;
    }

  d->m_MinimumPosition = minPosition;
  d->m_MaximumPosition = maxPosition;
  d->m_MainPosition = mainPosition;

  if (!this->hasTracking())
    {
    this->update();
    }
  if (isSliderDown())
    {
    if (emitMinPosChanged)
      {
      emit minimumPositionChanged(d->m_MinimumPosition);
      }
    if (emitMaxPosChanged)
      {
      emit maximumPositionChanged(d->m_MaximumPosition);
      }
    if (emitMainPosChanged)
      {
      emit mainPositionChanged(d->m_MainPosition);
      }
    if (emitMinPosChanged || emitMaxPosChanged || emitMainPosChanged)
      {
      emit positionsChanged(d->m_MinimumPosition, d->m_MaximumPosition, d->m_MainPosition);
      }
    }
  if (this->hasTracking())
    {
    this->triggerAction(SliderMove);
    this->setValues(d->m_MinimumPosition, d->m_MaximumPosition, d->m_MainPosition);
    }
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setSymmetricMoves(bool symmetry)
{
  Q_D(ctkRangeSlider);
  d->m_SymmetricMoves = symmetry;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::symmetricMoves()const
{
  Q_D(const ctkRangeSlider);
  return d->m_SymmetricMoves;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::onRangeChanged(int _minimum, int _maximum)
{
  Q_UNUSED(_minimum);
  Q_UNUSED(_maximum);
  Q_D(ctkRangeSlider);
  this->setValues(d->m_MinimumValue, d->m_MaximumValue, d->m_MainValue);
}

// --------------------------------------------------------------------------
// Render
void ctkRangeSlider::paintEvent( QPaintEvent* )
{
  Q_D(ctkRangeSlider);
  QStyleOptionSlider option;
  this->initStyleOption(&option);

  QStylePainter painter(this);
  option.subControls = QStyle::SC_SliderGroove;
  // Move to minimum to not highlight the SliderGroove.
  // On mac style, drawing just the slider groove also draws the handles,
  // therefore we give a negative (outside of view) position.
  option.sliderValue = this->minimum() - this->maximum();
  option.sliderPosition = this->minimum() - this->maximum();
  painter.drawComplexControl(QStyle::CC_Slider, option);

  option.sliderPosition = d->m_MinimumPosition;
  const QRect lr = style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            this);

  option.sliderPosition = d->m_MaximumPosition;
  const QRect ur = style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            this);

  QRect sr = style()->subControlRect( QStyle::CC_Slider,
                                      &option,
                                      QStyle::SC_SliderGroove,
                                      this);

  option.sliderPosition = d->m_MainPosition;
  const QRect mainr = style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            this);
  QRect rangeBox, mainBox;
  if (option.orientation == Qt::Horizontal) {
    if (!d->m_maxInRange && d->m_minInRange) {
      rangeBox = QRect(
        QPoint(lr.center().x(), sr.center().y() -10),
        QPoint(sr.right()+20, sr.center().y() + 10));
    }
    else if (d->m_maxInRange && !d->m_minInRange) {
      rangeBox = QRect(
        QPoint(sr.left()-20, sr.center().y() -10),
        QPoint(ur.center().x(), sr.center().y() + 10));
    }
    else if (!d->m_maxInRange && !d->m_minInRange) {
      rangeBox = QRect(
        QPoint(sr.left()-20, sr.center().y() -10),
        QPoint(sr.right()+20, sr.center().y() + 10));
    }
    else {
      rangeBox = QRect(
        QPoint(lr.center().x(), sr.center().y() -10),
        QPoint(ur.center().x(), sr.center().y() + 10));
      }
    }
  else
    {
      rangeBox = QRect(
        QPoint(sr.center().x() - 2, qMin( lr.center().y(), ur.center().y() )),
        QPoint(sr.center().x() + 1, qMax( lr.center().y(), ur.center().y() )));
    }

  if (!d->m_mainOverRange) {
    mainBox = QRect(
      QPoint(0, sr.center().y()),
      QPoint(mainr.center().x(), sr.center().y()));
  }
  else {
    mainBox = QRect(
      QPoint(0, sr.center().y()),
      QPoint(sr.right()+20, sr.center().y()));
  }


  // -----------------------------
  // Render the range
  //
  QRect groove = this->style()->subControlRect( QStyle::CC_Slider,
                                                &option,
                                                QStyle::SC_SliderGroove,
                                                this );
  groove.adjust(0, 0, -1, 0);

  // Create default colors based on the transfer function.
  //
  QColor highlight = this->palette().color(QPalette::Normal, QPalette::Highlight);
  QColor windowColor(32,100,164);
  QLinearGradient gradient;
  /*if (option.orientation == Qt::Horizontal)
    {
    gradient = QLinearGradient( groove.center().x(), groove.top(),
                                groove.center().x(), groove.bottom());
    }
  else
    {
    gradient = QLinearGradient( groove.left(), groove.center().y(),
                                groove.right(), groove.center().y());
    }*/

  // TODO: Set this based on the supplied transfer function
  //QColor l = Qt::darkGray;
  //QColor u = Qt::black;

  //gradient.setColorAt(0, highlight.darker(120));
  //gradient.setColorAt(1, highlight.lighter(160));

  if (d->m_rangeVisible) {
    painter.setPen(QPen(windowColor, 5));
    //painter.setBrush(gradient);
    painter.drawRect( rangeBox);
  }
  if (d->m_mainVisible) {
    painter.setPen(QPen(highlight, 0));
    painter.drawRect( mainBox);
  }

  //  -----------------------------------
  // Render the sliders
  //
  if (d->m_rangeVisible) {
    if (d->m_maxInRange) {
      d->drawMaximumSlider( &painter );
    }
    if (d->m_minInRange) {
      d->drawMinimumSlider( &painter );
    }
  }
  if (d->m_mainVisible &&  !d->m_mainOverRange) {
    d->drawMainSlider( &painter );
  }
}

// --------------------------------------------------------------------------
// Standard Qt UI events
void ctkRangeSlider::mousePressEvent(QMouseEvent* mouseEvent)
{
  Q_D(ctkRangeSlider);
  if (minimum() == maximum() || (mouseEvent->buttons() ^ mouseEvent->button()))
    {
    mouseEvent->ignore();
    return;
    }
  int mepos = this->orientation() == Qt::Horizontal ?
    mouseEvent->pos().x() : mouseEvent->pos().y();

  QStyleOptionSlider option;
  this->initStyleOption( &option );

  QRect handleRect;
  ctkRangeSliderPrivate::Handle handle_ = d->handleAtPos(mouseEvent->pos(), handleRect);

  if (handle_ != ctkRangeSliderPrivate::NoHandle)
    {
    if (handle_ == ctkRangeSliderPrivate::MinimumHandle && d->m_rangeVisible && d->m_minInRange) d->m_SubclassPosition = ctkRangeSliderPrivate::MinimumHandle;
    else if (handle_ == ctkRangeSliderPrivate::MaximumHandle && d->m_rangeVisible && d->m_maxInRange) d->m_SubclassPosition = ctkRangeSliderPrivate::MaximumHandle;
    else if (handle_ == ctkRangeSliderPrivate::MainHandle && d->m_mainVisible) d->m_SubclassPosition = ctkRangeSliderPrivate::MainHandle;

    // save the position of the mouse inside the handle for later
    d->m_SubclassClickOffset = mepos - (this->orientation() == Qt::Horizontal ?
      handleRect.left() : handleRect.top());

    this->setSliderDown(true);

    if (d->m_SelectedHandles != handle_)
      {
      d->m_SelectedHandles = handle_;
      this->update(handleRect);
      }
    // Accept the mouseEvent
    mouseEvent->accept();
    return;
    }

  // if we are here, no handles have been pressed
  // Check if we pressed on the groove between the 2 handles

  QStyle::SubControl control = this->style()->hitTestComplexControl(
    QStyle::CC_Slider, &option, mouseEvent->pos(), this);
  QRect sr = style()->subControlRect(
    QStyle::CC_Slider, &option, QStyle::SC_SliderGroove, this);
  int minCenter = (this->orientation() == Qt::Horizontal ?
    handleRect.left() : handleRect.top());
  int maxCenter = (this->orientation() == Qt::Horizontal ?
    handleRect.right() : handleRect.bottom());
  if (control == QStyle::SC_SliderGroove &&
      mepos > minCenter && mepos < maxCenter)
    {
    // warning lost of precision it might be fatal
    d->m_SubclassPosition = (d->m_MinimumPosition + d->m_MaximumPosition) / 2.;
    d->m_SubclassClickOffset = mepos - d->pixelPosFromRangeValue(d->m_SubclassPosition);
    d->m_SubclassWidth = (d->m_MaximumPosition - d->m_MinimumPosition) / 2;
    qMax(d->m_SubclassPosition - d->m_MinimumPosition, d->m_MaximumPosition - d->m_SubclassPosition);
    this->setSliderDown(true);
    if (!this->isMinimumSliderDown() || !this->isMaximumSliderDown())
      {
        if (d->m_rangeVisible) {
          d->m_SelectedHandles =
          QFlags<ctkRangeSliderPrivate::Handle>(ctkRangeSliderPrivate::MinimumHandle) |
          QFlags<ctkRangeSliderPrivate::Handle>(ctkRangeSliderPrivate::MaximumHandle);
        }
        else {
          d->m_SelectedHandles = 0x0;
        }
      this->update(handleRect.united(sr));
      }
    mouseEvent->accept();
    return;
    }
  mouseEvent->ignore();
}

// --------------------------------------------------------------------------
// Standard Qt UI events
void ctkRangeSlider::mouseMoveEvent(QMouseEvent* mouseEvent)
{


  Q_D(ctkRangeSlider);
  if (!d->m_SelectedHandles)
    {
    mouseEvent->ignore();
    return;
    }
  int mepos = this->orientation() == Qt::Horizontal ?
    mouseEvent->pos().x() : mouseEvent->pos().y();

  QStyleOptionSlider option;
  this->initStyleOption(&option);

  const int m = style()->pixelMetric( QStyle::PM_MaximumDragDistance, &option, this );

  int newPosition = d->pixelPosToRangeValue(mepos - d->m_SubclassClickOffset);

  if (m >= 0)
    {
    const QRect r = rect().adjusted(-m, -m, m, m);
    if (!r.contains(mouseEvent->pos()))
      {
      newPosition = d->m_SubclassPosition;
      }
    }

  // Only the lower/left slider is down
  if (this->isMinimumSliderDown() && !this->isMaximumSliderDown() && !this->isMainSliderDown())
    {
    double newMinPos = qMin(newPosition,d->m_MaximumPosition);
    this->setPositions(newMinPos, d->m_MaximumPosition +
      (d->m_SymmetricMoves ? d->m_MinimumPosition - newMinPos : 0), d->m_MainPosition);
    }
  // Only the upper/right slider is down
  else if (this->isMaximumSliderDown() && !this->isMinimumSliderDown() && !this->isMainSliderDown())
    {
    double newMaxPos = qMax(d->m_MinimumPosition, newPosition);
    this->setPositions(d->m_MinimumPosition -
      (d->m_SymmetricMoves ? newMaxPos - d->m_MaximumPosition: 0),
      newMaxPos, d->m_MainPosition);
    }
  // Only the upper/right slider is down
  else if (this->isMainSliderDown())
    {
    double newMaxPos = qMax(d->m_MinimumPosition, newPosition);
    this->setPositions(d->m_MinimumPosition, d->m_MaximumPosition, newPosition);
    }
  // Both handles are down (the user clicked in between the handles)
  else if (this->isMinimumSliderDown() && this->isMaximumSliderDown() && !this->isMainSliderDown())
    {
    this->setPositions(newPosition - d->m_SubclassWidth,
                       newPosition + d->m_SubclassWidth , d->m_MainPosition);
    }
  mouseEvent->accept();
}

// --------------------------------------------------------------------------
// Standard Qt UI mouseEvents
void ctkRangeSlider::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
  Q_D(ctkRangeSlider);
  this->QSlider::mouseReleaseEvent(mouseEvent);

  setSliderDown(false);
  d->m_SelectedHandles = ctkRangeSliderPrivate::NoHandle;

  this->update();
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::isMinimumSliderDown()const
{
  Q_D(const ctkRangeSlider);
  return d->m_SelectedHandles & ctkRangeSliderPrivate::MinimumHandle;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::isMaximumSliderDown()const
{
  Q_D(const ctkRangeSlider);
  return d->m_SelectedHandles & ctkRangeSliderPrivate::MaximumHandle;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::isMainSliderDown()const
{
  Q_D(const ctkRangeSlider);
  return d->m_SelectedHandles & ctkRangeSliderPrivate::MainHandle;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::initMinimumSliderStyleOption(QStyleOptionSlider* option) const
{
  this->initStyleOption(option);
}

// --------------------------------------------------------------------------
void ctkRangeSlider::initMaximumSliderStyleOption(QStyleOptionSlider* option) const
{
  this->initStyleOption(option);
}

// --------------------------------------------------------------------------
void ctkRangeSlider::initMainSliderStyleOption(QStyleOptionSlider* option) const
{
  this->initStyleOption(option);
}

// --------------------------------------------------------------------------
QString ctkRangeSlider::handleToolTip()const
{
  Q_D(const ctkRangeSlider);
  return d->m_HandleToolTip;
}

// --------------------------------------------------------------------------
void ctkRangeSlider::setHandleToolTip(const QString& _toolTip)
{
  Q_D(ctkRangeSlider);
  d->m_HandleToolTip = _toolTip;
}

// --------------------------------------------------------------------------
bool ctkRangeSlider::event(QEvent* _event)
{
  Q_D(ctkRangeSlider);
  switch(_event->type())
    {
    case QEvent::ToolTip:
      {
      QHelpEvent* helpEvent = static_cast<QHelpEvent*>(_event);
      QStyleOptionSlider opt;
      // Test the MinimumHandle
      opt.sliderPosition = d->m_MinimumPosition;
      opt.sliderValue = d->m_MinimumValue;
      this->initStyleOption(&opt);
      QStyle::SubControl hoveredControl =
        this->style()->hitTestComplexControl(
          QStyle::CC_Slider, &opt, helpEvent->pos(), this);
      if (!d->m_HandleToolTip.isEmpty() &&
          hoveredControl == QStyle::SC_SliderHandle)
        {
        QToolTip::showText(helpEvent->globalPos(), d->m_HandleToolTip.arg(this->minimumValue()));
        _event->accept();
        return true;
        }
      // Test the MaximumHandle
      opt.sliderPosition = d->m_MaximumPosition;
      opt.sliderValue = d->m_MaximumValue;
      this->initStyleOption(&opt);
      hoveredControl = this->style()->hitTestComplexControl(
        QStyle::CC_Slider, &opt, helpEvent->pos(), this);
      if (!d->m_HandleToolTip.isEmpty() &&
          hoveredControl == QStyle::SC_SliderHandle)
        {
        QToolTip::showText(helpEvent->globalPos(), d->m_HandleToolTip.arg(this->maximumValue()));
        _event->accept();
        return true;
        }
      }
    default:
      break;
    }
  return this->Superclass::event(_event);
}
