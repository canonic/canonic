#include "../include/QQuickMainWindow.hpp"

#include "../include/Window.hpp"
#include "../include/CxxSyntaxHighlighter.hpp"
#include "../include/URISyntaxHighlighter.hpp"

#include <QQmlEngine>
#include <QtQml>

#include <iostream>


class QuickViewInternal: public QQuickView
{

  public:
    QuickViewInternal(QQuickMainWindow * qQuickMainWindow): m_qQuickMainWindow{qQuickMainWindow}
    {
    }

    void resizeEvent(QResizeEvent* event)
    {
        QQuickView::resizeEvent(event);

        this->m_qQuickMainWindow->m_window->handleWindowResize(event);
        emit this->m_qQuickMainWindow->onResize(event);
    }

  private:
    QQuickMainWindow *m_qQuickMainWindow;
};

QQuickMainWindow::QQuickMainWindow() : MainWindow{}
{
    this->m_qQuickView = new QuickViewInternal(this);
    this->m_qQuickView->setResizeMode(QQuickView::SizeRootObjectToView);

    /**
     * Main Viewport
     **/

    // Get the qml engine's rootContext
    QQmlContext *mainUIRootContext{this->m_qQuickView->rootContext()};

    // Get the underlying engine
    QQmlEngine *mainUIQmlEngine = mainUIRootContext->engine();
    mainUIQmlEngine->addImportPath("qrc:/");

    // Set the qml engine's networkAccessManagerFactory
    mainUIQmlEngine->setNetworkAccessManagerFactory(this->m_networkAccessManagerFactory);

    qmlRegisterUncreatableType<HistoryItem>("com.mycompany.qmlcomponents", 1, 0, "HistoryItem", "Can not create HistoryItem");
    qmlRegisterType<View>("com.mycompany.qmlcomponents", 1, 0, "View");

    mainUIRootContext->setContextProperty("window", this->m_window);
    mainUIRootContext->setContextProperty("mainWindow", this);
    qmlRegisterType< CxxSyntaxHighlighter >( "Meta", 1, 0, "CxxSyntaxHighlighter" );
    qmlRegisterType< URISyntaxHighlighter >( "Meta", 1, 0, "URISyntaxHighlighter" );

    //this->m_mainViewport->setAttribute(Qt::WA_AlwaysStackOnTop);
    //this->m_mainViewport->setClearColor(Qt::transparent);
    this->m_qQuickView->setSource(QStringLiteral("qrc:/qml/MainUIQQuick.qml"));
}

QQuickMainWindow::~QQuickMainWindow()
{
    // Set the sources to an empty url so that no errors are thrown when context object are deleted
    this->m_qQuickView->setSource(QUrl());
}


const int QQuickMainWindow::width() const
{
    return this->m_qQuickView->width();
}

const int QQuickMainWindow::height() const
{
    return this->m_qQuickView->height();
}

void QQuickMainWindow::resize(int w, int h)
{
    this->m_qQuickView->resize(w, h);
}

void QQuickMainWindow::setGeometry(int x, int y, int w, int h)
{
    this->m_qQuickView->setGeometry(x, y, w, h);
}

void QQuickMainWindow::show()
{
    this->m_qQuickView->show();
}

QQmlEngine* QQuickMainWindow::getQmlEngine() const
{
    return this->m_qQuickView->rootContext()->engine();
}

void QQuickMainWindow::refresh()
{
    QUrl tmp = this->m_qQuickView->source();
    qDebug() << "refresh main window";
    this->m_qQuickView->setSource(QUrl(""));
    this->m_qQuickView->rootContext()->engine()->clearComponentCache();
    this->resetThemeComponent();
    this->m_qQuickView->setSource(tmp);
}

void QQuickMainWindow::handleActiveViewChanged(View *view)
{
    this->m_qQuickView->setSource(view->getQmlSource());
    QQuickMainWindow::handleActiveViewChanged(view);
}
