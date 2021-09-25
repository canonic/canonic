#include "../include/ContentViewport.hpp"
#include "../include/MainWindow.hpp"

ContentViewport::ContentViewport(MainWindow *mainWindow,
                                 RenderControl *renderControl):
    Viewport(mainWindow, renderControl)
{
    this->setObjectName("ContentViewport");
    //connect(this, &Viewport::initalised, this, &ContentViewport::reloadTLISource);
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
