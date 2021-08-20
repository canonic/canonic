#include "../include/QWidgetMainWindow.hpp"
#include "../include/Location.hpp"
#include "../include/MainWindow.hpp"
#include "../include/View.hpp"
#include "../include/Window.hpp"

#include "../include/CxxSyntaxHighlighter.hpp"
#include "../include/URISyntaxHighlighter.hpp"

#include <QQmlEngine>
#include <QtQml>

#include <iostream>


class MainWindowInternal: public QMainWindow
{

  public:
    MainWindowInternal(QWidgetMainWindow * qWidgetMainWindow): m_qWidgetMainWindow{qWidgetMainWindow}
    {
    }

    void resizeEvent(QResizeEvent* event)
    {
        QMainWindow::resizeEvent(event);

        this->m_qWidgetMainWindow->m_window->handleWindowResize(event);

        this->m_qWidgetMainWindow->getLayoutViewport()->setGeometry(this->m_qWidgetMainWindow->m_window->getInnerScreenX(),
            this->m_qWidgetMainWindow->m_window->getInnerScreenY(),
            this->m_qWidgetMainWindow->m_window->getInnerWidth(),
            this->m_qWidgetMainWindow->m_window->getInnerHeight());

        this->m_qWidgetMainWindow->getMainViewport()->setGeometry(0, 0, event->size().width(), event->size().height());

        emit this->m_qWidgetMainWindow->onResize(event);
    }

    private:
        QWidgetMainWindow *m_qWidgetMainWindow;
};

QWidgetMainWindow::QWidgetMainWindow() : MainWindow{}
{
    this->m_qMainWindow = new MainWindowInternal(this);

    QWidget *centralWidget = new QWidget();
    this->m_qMainWindow->setCentralWidget(centralWidget);

    /**
     * Layout Viewport
    **/
    this->m_layoutViewport = new LayoutViewport();
    this->m_layoutViewport->setResizeMode(QQuickWidget::SizeRootObjectToView);
    this->m_layoutViewport->setClearColor("#191c21");

    QQmlEngine *browserQmlEngine = this->m_layoutViewport->engine();

    // Set the qml engine's networkAccessManagerFactory
    browserQmlEngine->setNetworkAccessManagerFactory(this->m_networkAccessManagerFactory);
    browserQmlEngine->addImportPath("qrc:/");

    // Get the qml engine's rootContext
    QQmlContext *browserRootContext{browserQmlEngine->rootContext()};
    browserRootContext->setContextProperty("window", this->m_window);
    qmlRegisterType< CxxSyntaxHighlighter >( "Meta", 1, 0, "CxxSyntaxHighlighter" );
    qmlRegisterType< URISyntaxHighlighter >( "Meta", 1, 0, "URISyntaxHighlighter" );

    this->m_layoutViewport->setSource(QStringLiteral(""));
    this->m_layoutViewport->setParent(centralWidget);

    /**
     * Main Viewport
     **/
    this->m_mainViewport = new MainViewport(this->m_layoutViewport);
    this->m_mainViewport->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QQmlEngine *mainUIQmlEngine = this->m_mainViewport->engine();
    mainUIQmlEngine->addImportPath("qrc:/");

    // Set the qml engine's networkAccessManagerFactory
    mainUIQmlEngine->setNetworkAccessManagerFactory(this->m_networkAccessManagerFactory);

    // Get the qml engine's rootContext
    QQmlContext *mainUIRootContext{mainUIQmlEngine->rootContext()};
    qmlRegisterUncreatableType<HistoryItem>("com.mycompany.qmlcomponents", 1, 0, "HistoryItem", "Can not create HistoryItem");
    qmlRegisterType<View>("com.mycompany.qmlcomponents", 1, 0, "View");
    mainUIRootContext->setContextProperty("window", this->m_window);
    mainUIRootContext->setContextProperty("mainWindow", this);

    this->m_mainViewport->setAttribute(Qt::WA_AlwaysStackOnTop);
    this->m_mainViewport->setClearColor(Qt::transparent);
    this->m_mainViewport->setSource(QStringLiteral("qrc:/qml/MainUIQWidget.qml"));

    // Set the Main ui as the parent so mouse events propagate to the main ui when not accepted
    this->m_mainViewport->setParent(centralWidget);

    this->connect(this->m_layoutViewport, &QQuickWidget::statusChanged,
        this, &QWidgetMainWindow::handleLayoutViewStatusChanged);

    this->connect(this, &MainWindow::activeViewIndexChanged,
        this, &QWidgetMainWindow::handleActiveViewIndexChanged);
}

QWidgetMainWindow::~QWidgetMainWindow()
{
    // Set the sources to an empty url so that no errors are thrown when context object are deleted
    this->m_layoutViewport->setSource(QUrl());
    this->m_mainViewport->setSource(QUrl());
}

const int QWidgetMainWindow::width() const
{
    return this->m_qMainWindow->width();
}

const int QWidgetMainWindow::height() const
{
    return this->m_qMainWindow->height();
}

void QWidgetMainWindow::resize(int w, int h)
{
    std::cout << "resizing window" << std::endl;
    this->m_qMainWindow->resize(w, h);
    std::cout << "resizing finished " << std::endl;
}

void QWidgetMainWindow::setGeometry(int x, int y, int w, int h)
{
    std::cout << "setGeometry:" << x << "   " << y << "   " << w << "   " << h << "   " << std::endl;
    this->m_qMainWindow->setGeometry(x, y, w, h);
}

void QWidgetMainWindow::show()
{
    this->m_qMainWindow->show();
}

void QWidgetMainWindow::setActiveViewIndex(int activeViewIndex)
{
    this->m_layoutViewport->setSource(QUrl(""));
    MainWindow::setActiveViewIndex(activeViewIndex);
}

LayoutViewport* QWidgetMainWindow::getLayoutViewport() const
{
    return this->m_layoutViewport;
}

MainViewport* QWidgetMainWindow::getMainViewport() const
{
    return this->m_mainViewport;
}

QQmlEngine* QWidgetMainWindow::getQmlEngine() const
{
    return this->m_layoutViewport->engine();
}

void QWidgetMainWindow::handleMouseEvent(QEvent::Type type,
    QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
    QPointF windowPos = localPos;
    QPointF globalPos = this->m_mainViewport->mapToGlobal(localPos.toPoint());
    localPos -= this->m_layoutViewport->pos();

    QMouseEvent* evt = new QMouseEvent(type,
        localPos,
        windowPos,
        globalPos,
        (Qt::MouseButton)button,
        (Qt::MouseButtons)buttons,
        (Qt::KeyboardModifiers)modifiers,
        (Qt::MouseEventSource)source);

    QCoreApplication::postEvent(this->m_layoutViewport, evt);
}


void QWidgetMainWindow::handleMouseDoubleClickEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
    this->m_layoutViewport->setFocus();
    this->handleMouseEvent(QEvent::MouseButtonDblClick,
        localPos,
        button,
        buttons,
        modifiers,
        source);
}

void QWidgetMainWindow::handlePositionChangedEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
    this->handleMouseEvent(QEvent::MouseMove,
        localPos,
        button,
        buttons,
        modifiers,
        source);
}

void QWidgetMainWindow::handlePressEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
    this->m_layoutViewport->setFocus();
    this->handleMouseEvent(QEvent::MouseButtonPress,
        localPos,
        button,
        buttons,
        modifiers,
        source);
}

void QWidgetMainWindow::handleReleaseEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
    this->handleMouseEvent(QEvent::MouseButtonRelease,
        localPos,
        button,
        buttons,
        modifiers,
        source);
}

void QWidgetMainWindow::handleWheelEvent(QPointF localPos,
    QPoint pixelDelta,
    QPoint angleDelta,
    int buttons,
    int modifiers,
    bool inverted,
    int source)
{
    QPointF globalPos = this->m_mainViewport->mapToGlobal(localPos.toPoint());
    localPos -= this->m_layoutViewport->pos();

    QWheelEvent* evt = new QWheelEvent(localPos,
        globalPos,
        pixelDelta,
        angleDelta,
        (Qt::MouseButtons)buttons,
        (Qt::KeyboardModifiers)modifiers,
        Qt::ScrollUpdate,
        inverted,
        (Qt::MouseEventSource)source);

    QCoreApplication::postEvent(this->m_layoutViewport, evt);
}

// Can possibly merge with setActiveViewIndex
void QWidgetMainWindow::handleActiveViewIndexChanged(int activeViewIndex)
{
    if(activeViewIndex >= 0 && activeViewIndex < this->viewCount())
    {
        this->m_layoutViewport->setSource(this->view(activeViewIndex)->getQmlSource());
    }
    else {
        this->m_layoutViewport->setSource(QUrl(""));
    }
}

void QWidgetMainWindow::handleLayoutViewStatusChanged(QQuickWidget::Status status)
{
    /*
    switch(status)
    {
        case QQuickWidget::Error:
        {
            for (QQmlError error : this->m_layoutViewport->errors())
            {
                qDebug() << error.toString();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    */
}
