#pragma once

#include "./LayoutViewport.hpp"

class MainViewport: public QQuickWidget
{
  private:
    LayoutViewport *m_unhandledEventHandler;

    /*
  protected:
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent  *e) override;
    void tabletEvent(QTabletEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    */

  public:
    MainViewport(LayoutViewport * unhandledEventHandler);
};
