#pragma once

#include "MainWindow.fwd.hpp"
#include "RenderControl.fwd.hpp"
#include "Viewport.hpp"

/**
 * The ContentViewport class is a QQuickWindow responsible for rendering the
 * currently loaded document.
 **/
class ContentViewport: public Viewport
{
  public:
    explicit ContentViewport(MainWindow *mainWindow,
                             RenderControl *renderControl);


    void setTLISource(QUrl source);
    void reloadTLISource();
};
