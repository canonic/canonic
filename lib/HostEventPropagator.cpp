#include "../include/HostEventPropagator.hpp"
#include "../include/ContentViewport.hpp"
#include "../include/HostViewport.hpp"

ContentViewport * HostEventPropagator::m_contentViewport = nullptr;
HostViewport * HostEventPropagator::m_hostViewport = nullptr;

HostEventPropagator::HostEventPropagator()
{
    setObjectName("HostEventPropagator");
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setFlag(ItemAcceptsInputMethod, true);
    setFlag(ItemIsFocusScope, true);
    setKeepMouseGrab(true);
}

void HostEventPropagator::keyPressEvent(QKeyEvent *event)
{
    QCoreApplication::sendEvent(this->m_contentViewport, event);
}

void HostEventPropagator::keyReleaseEvent(QKeyEvent *event)
{
    QCoreApplication::sendEvent(this->m_contentViewport, event);
}

void HostEventPropagator::mousePressEvent(QMouseEvent *event)
{
    emit this->clearHostViewportFocus();

    HostEventPropagator::m_hostViewport->enableForwardEvents();

    QCoreApplication::sendEvent(m_hostViewport, event);
}

void HostEventPropagator::wheelEvent(QWheelEvent *event)
{
    HostEventPropagator::m_hostViewport->enableForwardEvents();

    QCoreApplication::sendEvent(m_hostViewport, event);
}

void HostEventPropagator::hoverHandler(QHoverEvent* event) const
{
    // Not sure why but hover events don't work properly unless we create a
    // unused hover handler with the same data
    QHoverEvent(event->type(), event->position(), event->oldPosF(),
                event->modifiers(), event->pointingDevice());

    QCoreApplication::sendEvent(m_contentViewport, event);
}

void HostEventPropagator::hoverEnterEvent(QHoverEvent* event)
{
    this->hoverHandler(event);
}

void HostEventPropagator::hoverLeaveEvent(QHoverEvent* event)
{
    this->hoverHandler(event);
}

void HostEventPropagator::hoverMoveEvent(QHoverEvent* event)
{
    this->hoverHandler(event);
}
