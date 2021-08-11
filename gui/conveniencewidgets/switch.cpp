#include "switch.hpp"

Switch::Switch(QWidget *parent) : QAbstractButton(parent),
_height(16),
_opacity(0.000),
_switch(false),
_margin(3),
_thumb(QColor(32, 100, 164)),
_anim(new QPropertyAnimation(this, "offset", this))
{
    setOffset(_height / 2);
    _y = _height / 2;
    setBrush(QColor(100,164,32));
}

Switch::Switch(const QBrush &brush, QWidget *parent) : QAbstractButton(parent),
_height(16),
_switch(false),
_opacity(0.000),
_margin(3),
_thumb(QColor(32, 100, 164)),
_anim(new QPropertyAnimation(this, "offset", this))
{
    setOffset(_height / 2);
    _y = _height / 2;
    setBrush(brush);
}

void Switch::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (isEnabled()) {
        p.setBrush(_switch ? brush() : QColor(22, 24, 26));
        p.setOpacity(_switch ? 0.5 : 0.38);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(QRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin), 8.0, 8.0);
        p.setBrush(_thumb);
        p.setOpacity(1.0);
        p.drawEllipse(QRectF(offset() - (_height / 2), _y - (_height / 2), height(), height()));
    } else {
        p.setBrush(QColor(22, 24, 26));
        p.setOpacity(0.12);
        p.drawRoundedRect(QRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin), 8.0, 8.0);
        p.setOpacity(1.0);
        p.setBrush(QColor("#BDBDBD"));
        p.drawEllipse(QRectF(offset() - (_height / 2), _y - (_height / 2), height(), height()));
    }
}

void Switch::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        _switch = _switch ? false : true;
        _thumb = _switch ? _brush : QBrush(QColor(32, 100, 164));
        if (_switch) {
            _anim->setStartValue(_height / 2);
            _anim->setEndValue(width() - _height);
            _anim->setDuration(120);
            _anim->start();
        } else {
            _anim->setStartValue(offset());
            _anim->setEndValue(_height / 2);
            _anim->setDuration(120);
            _anim->start();
        }
    }
    emit toggled(_switch);
    QAbstractButton::mouseReleaseEvent(e);
}

void Switch::enterEvent(QEvent *e) {
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(e);
}

QSize Switch::sizeHint() const {
    return QSize(2 * (_height + _margin), _height + 2 * _margin);
}

void Switch::setToggled(bool toggle) {
  _switch = toggle;
  _thumb = _switch ? _brush : QBrush(QColor(32, 100, 164));
  if (_switch) {
    _anim->setStartValue(_height / 2);
    _anim->setEndValue(width() - _height);
    _anim->setDuration(120);
    _anim->start();
  }
  else {
    _anim->setStartValue(offset());
    _anim->setEndValue(_height / 2);
    _anim->setDuration(120);
    _anim->start();
  }
}
