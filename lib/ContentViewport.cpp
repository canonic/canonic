#include "../include/ContentViewport.hpp"
#include "../include/MainWindow.hpp"
#include "../include/Window.hpp"

ContentViewport::ContentViewport(MainWindow *mainWindow,
                                 RenderControl *renderControl):
    Viewport(mainWindow, renderControl)
{
    this->setObjectName("ContentViewport");
    //connect(this, &Viewport::initalised, this, &ContentViewport::reloadTLISource);
}

void ContentViewport::updateItemSizes(QSize newSize)
{
    // Set the QQUickWindows content items size
    QQuickItem *contentItem = this->contentItem();
    if (contentItem != nullptr){
        contentItem->setWidth(newSize.width());
        contentItem->setHeight(newSize.height());
        contentItem->setX(this->m_mainWindow->m_window->getInnerScreenX());
        contentItem->setY(this->m_mainWindow->m_window->getInnerScreenY());
    }

    // Set the root items items size
    if (m_rootItem != nullptr){
        this->m_rootItem->setWidth(newSize.width());
        this->m_rootItem->setHeight(newSize.height());
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
