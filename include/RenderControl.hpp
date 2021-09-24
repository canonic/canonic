#pragma once

#include <QQuickRenderControl>
#include <QWindow>

class RenderControl : public QQuickRenderControl
{
  public:
    RenderControl(QWindow *w);
    QWindow *renderWindow(QPoint *offset) override;

  private:
    QWindow *m_window;
};

