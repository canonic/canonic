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
    //connect(this, &Viewport::initalised, this, &ContentViewport::reloadTLISource);
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
}

void ContentViewport::setTLISource(QUrl source)
{
    /*
    qDebug() << "setTLISource:" << source;
    if(this->m_rootItem != nullptr){
        QUrl tmp = this->m_rootItem->property("source").toString();

        // this is causing WebGL: INVALID_OPERATION: drawElements: no buffer is bound to enabled attribute
        // errors if called twice
        if(tmp == QUrl("")) {
            qDebug() << "setting";
            qDebug() << m_rootItem->setProperty("source", source);
        }
    }
    */
}

void ContentViewport::reloadTLISource()
{
    qDebug() << "reloadTLISource: " << (this->m_rootItem == nullptr);
    if(this->m_rootItem != nullptr){
        QUrl tmp = this->m_rootItem->property("source").toString();
        this->m_rootItem->setProperty("source", QUrl(""));
        //this->m_qmlEngine->clearComponentCache();
        //this->resetThemeComponent();
        this->m_rootItem->setProperty("source", tmp);
    }
}
