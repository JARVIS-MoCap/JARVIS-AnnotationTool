#ifndef BETTERCAMERACONTROLLER_H
#define BETTERCAMERACONTROLLER_H

#include "globals.hpp"

#include <Qt3DExtras/QOrbitCameraController>
#include <QPoint>
#include <QSize>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>


class BetterCameraController : public Qt3DExtras::QOrbitCameraController
{
    Q_OBJECT
public:
    BetterCameraController(Qt3DCore::QNode *parent = nullptr);

private:
    void moveCamera(const QAbstractCameraController::InputState &state, float dt) override;

};

#endif