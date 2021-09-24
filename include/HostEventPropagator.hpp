#pragma once

#include <QQuickItem>

#include "./ContentViewport.fwd.hpp"
#include "./HostViewport.fwd.hpp"

/**
 * The HostEventPropagator is a QQuickItem responsible for propagating events
 * from the HostViewport to the ContentViewport.
 **/
class HostEventPropagator : public QQuickItem
{
    Q_OBJECT

  signals:
    void clearHostViewportFocus();

  public:
    static ContentViewport *m_contentViewport;
    static HostViewport *m_hostViewport;

    HostEventPropagator();

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void hoverHandler(QHoverEvent* event) const;

    void hoverEnterEvent(QHoverEvent* event);

    void hoverLeaveEvent(QHoverEvent* event);

    void hoverMoveEvent(QHoverEvent* event);
};
