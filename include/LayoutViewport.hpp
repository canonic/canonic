#pragma once

#include <QQuickWidget>

class LayoutViewport: public QQuickWidget
{
  public:
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void tabletEvent(QTabletEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};
