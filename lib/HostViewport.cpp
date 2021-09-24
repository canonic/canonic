#include "../include/HostViewport.hpp"
#include "../include/MainWindow.hpp"

#include <QQmlContext>

HostViewport::HostViewport(MainWindow *mainWindow,
                           RenderControl *renderControl):
    Viewport(mainWindow, renderControl),
    m_forwardEvents{false}
{
    this->setObjectName("HostViewport");

    // Set host viewport specific context properties
    this->m_qmlEngine->rootContext()->setContextProperty("mainWindow", m_mainWindow);
}

void HostViewport::enableForwardEvents()
{
    this->m_forwardEvents = true;
}

void HostViewport::setForwardEvents(bool forwardEvents)
{
    this->m_forwardEvents = forwardEvents;
}

bool HostViewport::event(QEvent *e)
{
    //qDebug() << e << m_forwardEvents;
    if (this->m_forwardEvents)
    //if (true)
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
