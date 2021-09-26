#pragma once

#include "MainWindow.fwd.hpp"
#include "RenderControl.fwd.hpp"
#include "Viewport.hpp"

/**
 * The HostViewport class is a QQuickWindow responsible for rendering the
 * trusted browser QML components. It displays the browser UI.
 **/
class HostViewport: public Viewport
{
  public:
    explicit HostViewport(MainWindow *mainWindow,
                          RenderControl *renderControl);

    void updateItemSizes(QSize newSize) override;
    void enableForwardEvents();
    void setForwardEvents(bool forwardEvents);

    bool event(QEvent *e) override;

  private:
    bool m_forwardEvents;
};
