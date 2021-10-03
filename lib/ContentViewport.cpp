#include "../include/ContentViewport.hpp"
#include "../include/MainWindow.hpp"
#include "../include/Window.hpp"

ContentViewport::ContentViewport(MainWindow *mainWindow,
                                 RenderControl *renderControl):
    Viewport(mainWindow, renderControl)
{
    // Urls may not link to a document that specifies a background colour. In
    // those cases we should display a white background to be consistent with
    // current browsers and default qt qml behavior.
    this->setColor(Qt::white);

    this->setObjectName("ContentViewport");
}

void ContentViewport::updateItemSizes(QSize newSize)
{
    // Set the QQUickWindows content items size
    QQuickItem *contentItem = this->contentItem();
    if (contentItem != nullptr){
        contentItem->setWidth(this->m_mainWindow->m_window->getInnerWidth());
        contentItem->setHeight(this->m_mainWindow->m_window->getInnerHeight());
        contentItem->setX(this->m_mainWindow->m_window->getInnerScreenX());
        contentItem->setY(this->m_mainWindow->m_window->getInnerScreenY());
    }

    // Set the root items items size
    if (m_rootItem != nullptr){
        this->m_rootItem->setWidth(this->m_mainWindow->m_window->getInnerWidth());
        this->m_rootItem->setHeight(this->m_mainWindow->m_window->getInnerHeight());
    }
    else if (this->m_rootWindow != nullptr)
    {
        this->m_rootWindow->setGeometry(this->m_mainWindow->m_window->getInnerScreenX(),
                                        this->m_mainWindow->m_window->getInnerScreenY(),
                                        this->m_mainWindow->m_window->getInnerWidth(),
                                        this->m_mainWindow->m_window->getInnerHeight());
    }
}
