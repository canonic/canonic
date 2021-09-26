#include "../include/HostViewport.hpp"
#include "../include/MainWindow.hpp"

#include <QQmlContext>

HostViewport::HostViewport(MainWindow *mainWindow,
                           RenderControl *renderControl):
    Viewport(mainWindow, renderControl),
    m_forwardEvents{false}
{
    this->setColor(Qt::transparent);
    this->setObjectName("HostViewport");

    // Set host viewport specific context properties
    this->m_qmlEngine->rootContext()->setContextProperty("mainWindow", m_mainWindow);
}

void HostViewport::updateItemSizes(QSize newSize)
{
    // Set the QQUickWindows content items size
    QQuickItem *contentItem = this->contentItem();
    if (contentItem != nullptr){
        contentItem->setWidth(newSize.width());
        contentItem->setHeight(newSize.height());
    }

    // Set the root items items size
    if (m_rootItem != nullptr){
        this->m_rootItem->setWidth(newSize.width());
        this->m_rootItem->setHeight(newSize.height());
    }
}

void HostViewport::enableForwardEvents()
{
    this->m_forwardEvents = true;
    ContentViewport *contentViewport = this->m_mainWindow->m_contentViewport;
    if (contentViewport)
    {
        contentViewport->contentItem()->forceActiveFocus();
    }
}

void HostViewport::setForwardEvents(bool forwardEvents)
{
    this->m_forwardEvents = forwardEvents;
}

bool HostViewport::event(QEvent *e)
{
    if (this->m_forwardEvents)
    {
        switch(e->type())
        {
            case QEvent::Type::MouseButtonRelease:
            case QEvent::Type::Wheel:
            {
                this->m_forwardEvents = false;
                QCoreApplication::sendEvent(this->m_mainWindow->m_contentViewport, e);
                return true;
            }
            default: {
                QCoreApplication::sendEvent(this->m_mainWindow->m_contentViewport, e);
                return true;
            }
        }
    }

    bool accepted = QQuickWindow::event(e);
    return accepted;
}
