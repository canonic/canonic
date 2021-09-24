#include "../include/MainWindow.hpp"
#include "../include/Location.hpp"
#include "../include/MainWindow.hpp"
#include "../include/View.hpp"
#include "../include/Window.hpp"

#include "../include/CxxSyntaxHighlighter.hpp"
#include "../include/HostEventPropagator.hpp"
#include "../include/URISyntaxHighlighter.hpp"

#ifdef Q_OS_WASM
#include <emscripten.h>
#endif

#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QQmlEngine>
#include <QQuickGraphicsDevice>
#include <QQuickRenderTarget>
#include <QQuickWindow>
#include <QtQml>

#include <iostream>

MainWindow::MainWindow()
{
    this->resize(1600, 1000);

    this->m_auth = new Auth();
    this->m_networkAccessManagerFactory = new NetworkAccessManagerFactory(this);
    this->m_window = new WebAPI::Window(this);

    setSurfaceType(QSurface::OpenGLSurface);
    QSurfaceFormat format;
    // Qt Quick may need a depth and stencil buffer. Always make sure these are available.
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    format.setAlphaBufferSize(8);
    setFormat(format);

    m_context = new QOpenGLContext;
    m_context->setFormat(format);
    m_context->create();

    m_offscreenSurface = new QOffscreenSurface;
    // Pass m_context->format(), not format. Format does not specify and color
    // buffer sizes, while the context, that has just been created, reports a
    // format that has these values filled in. Pass this to the offscreen
    // surface to make sure it will be compatible with the context's
    // configuration.
    m_offscreenSurface->setFormat(m_context->format());
    m_offscreenSurface->create();

    m_canonicRenderer = new CanonicRenderer(m_context, m_offscreenSurface);

    // Create a QQuickWindow that is associated with out render control. Note
    // that this window never gets created or shown, meaning that it will never
    // get an underlying native (platform) window.
    m_contentViewport = new ContentViewport(this, new RenderControl(this));
    m_hostViewport = new HostViewport(this, new RenderControl(this));

    HostEventPropagator::m_contentViewport = m_contentViewport;
    HostEventPropagator::m_hostViewport = m_hostViewport;

    m_contentViewport->setDefaultAlphaBuffer(true);
    m_hostViewport->setDefaultAlphaBuffer(true);
    m_contentViewport->setColor(Qt::transparent);
    m_hostViewport->setColor(Qt::transparent);

    qmlRegisterUncreatableType<HistoryItem>("com.mycompany.qmlcomponents", 1, 0, "HistoryItem", "Can not create HistoryItem");
    qmlRegisterType<View>("com.mycompany.qmlcomponents", 1, 0, "View");
    qmlRegisterType<HostEventPropagator>("com.mycompany.qmlcomponents", 1, 0, "HostEventPropagator");
    qmlRegisterType< CxxSyntaxHighlighter >( "Meta", 1, 0, "CxxSyntaxHighlighter" );
    qmlRegisterType< URISyntaxHighlighter >( "Meta", 1, 0, "URISyntaxHighlighter" );

    // When Quick says there is a need to render, we will not render immediately. Instead,
    // a timer with a small interval is used to get better performance.
    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(5);
    connect(&m_updateTimer, &QTimer::timeout, this, &MainWindow::render);

    // Now hook up the signals. For simplicy we don't differentiate between
    // renderRequested (only render is needed, no sync) and sceneChanged (polish
    // and sync is needed too).
    connect(m_contentViewport, &Viewport::statusChanged, this, &MainWindow::handleContentViewportStatusChange);
    connect(m_contentViewport->getRenderControl(), &QQuickRenderControl::renderRequested, this, &MainWindow::requestUpdate);
    connect(m_contentViewport->getRenderControl(), &QQuickRenderControl::sceneChanged, this, &MainWindow::requestUpdate);

    connect(m_hostViewport, &Viewport::statusChanged, this, &MainWindow::handleHostViewportStatusChange);
    connect(m_hostViewport->getRenderControl(), &QQuickRenderControl::renderRequested, this, &MainWindow::requestUpdate);
    connect(m_hostViewport->getRenderControl(), &QQuickRenderControl::sceneChanged, this, &MainWindow::requestUpdate);

    // Just recreating the texture on resize is not sufficient, when moving
    // between screens with different devicePixelRatio the QWindow size may
    // remain the same but the texture dimension is to change regardless.
    connect(this, &QWindow::screenChanged, this, &MainWindow::handleScreenChange);

    m_hostViewport->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
}

MainWindow::~MainWindow()
{
    delete this->m_contentViewport;
    delete this->m_hostViewport;

    delete m_canonicRenderer;
    delete m_offscreenSurface;
    delete m_context;
}

QRectF MainWindow::getNormalisedViewportGeometry() const
{
    if (this->m_contentViewport == nullptr || this->m_hostViewport == nullptr) {
        return QRectF();
    }

    int mainViewportWidth = this->m_hostViewport->geometry().width();
    int mainViewportHeight = this->m_hostViewport->geometry().height();

    if (mainViewportWidth == 0 || mainViewportHeight == 0) {
        return QRectF();
    }

    QRectF viewportGeometry = this->m_contentViewport->geometry();
    QRectF normalisedViewportGeometry;
    normalisedViewportGeometry.setX(viewportGeometry.x() / mainViewportWidth);
    normalisedViewportGeometry.setY(viewportGeometry.y() / mainViewportHeight);
    normalisedViewportGeometry.setWidth(viewportGeometry.width() / mainViewportWidth);
    normalisedViewportGeometry.setHeight(viewportGeometry.height() / mainViewportHeight);

    return normalisedViewportGeometry;
}

void MainWindow::render()
{
    if (!m_context->makeCurrent(m_offscreenSurface))
        return;

    // Polish, synchronize and render the next frame (into our texture). In this
    // example everything happens on the same thread and therefore all three
    // steps are performed in succession from here. In a threaded setup the
    // render() call would happen on a separate thread.
    if (this->m_contentViewport->getStatus() == Viewport::Status::Ready)
    {
        this->m_contentViewport->getRenderControl()->beginFrame();
        this->m_contentViewport->getRenderControl()->polishItems();
        this->m_contentViewport->getRenderControl()->sync();
        this->m_contentViewport->getRenderControl()->render();
        this->m_contentViewport->getRenderControl()->endFrame();
    }

    if (this->m_contentViewport->getStatus() == Viewport::Status::Ready)
    {
        this->m_hostViewport->getRenderControl()->beginFrame();
        this->m_hostViewport->getRenderControl()->polishItems();
        this->m_hostViewport->getRenderControl()->sync();
        this->m_hostViewport->getRenderControl()->render();
        this->m_hostViewport->getRenderControl()->endFrame();
    }


    QOpenGLFramebufferObject::bindDefault();
    m_context->functions()->glFlush();

    // Get something onto the screen.
    m_canonicRenderer->render(this,
                              this->getNormalisedViewportGeometry(),
                              this->m_contentViewport->getTextureId(),
                              this->m_hostViewport->getTextureId());
}

void MainWindow::requestUpdate()
{
    if (!m_updateTimer.isActive())
        m_updateTimer.start();
}

void MainWindow::handleHostViewportStatusChange(Viewport::Status status) {
    qDebug() << "onHostViewportInitalised";
    if (status == Viewport::Status::Ready)
    {
        this->show();
        this->m_contentViewport->setSource(QUrl(QStringLiteral("qrc:/qml/TLI.qml")));
    }
}

void MainWindow::handleContentViewportStatusChange(Viewport::Status status) {
    qDebug() << "onContentViewportInitalised";
    //this->m_contentViewport->setSource(QUrl(QStringLiteral("qrc:/qml/TLI.qml")));

    if (status == Viewport::Status::Ready)
    {
        // Set the initial url if provided
        QString initialUrl = QString(qgetenv("CANONIC_INITIAL_URL"));
        if(initialUrl.length())
        {
            this->m_window->getLocation()->setHref(initialUrl);
        }
        else {
            this->m_window->getLocation()->setHref("");
        }

        this->mainUILoaded();
    }
}

void MainWindow::handleScreenChange()
{
    this->m_window->handleWindowResize();

    QSize newSize = size() * devicePixelRatio();
    this->m_contentViewport->resizeTexture(newSize);
    this->m_hostViewport->resizeTexture(newSize);

    this->render();
}

void MainWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed()) {
        this->render();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    this->m_window->handleWindowResize();

    QSize newSize = size() * devicePixelRatio();
    this->m_contentViewport->resizeTexture(newSize);
    this->m_hostViewport->resizeTexture(newSize);

    this->render();
}

bool MainWindow::event(QEvent *event)
{
    if (event->isInputEvent()) {
        QCoreApplication::sendEvent(m_hostViewport, event);
        return true;
    }

    return QWindow::event(event);
}

void MainWindow::previousPage()
{
    std::cout << "previousPage" << this->m_historyIndex << std::endl;
    if(this->m_historyIndex > 0)
    {
        std::cout << "chaning location" << std::endl;
        this->m_shouldIncrementHistoryIndex = false;

        this->m_historyIndex--;
        emit this->historyIndexChanged();

        this->m_window->getLocation()->setHref(this->historyItem(this->m_historyIndex)->getLocation()->getHref());
    }
}

void MainWindow::nextPage()
{
    std::cout << "nextPage" << this->m_historyIndex << std::endl;
    if(this->m_historyIndex < this->historyItemsCount() - 1)
    {
        std::cout << "chaning location" << std::endl;
        this->m_shouldIncrementHistoryIndex = false;

        this->m_historyIndex++;
        emit this->historyIndexChanged();

        this->m_window->getLocation()->setHref(this->historyItem(this->m_historyIndex)->getLocation()->getHref());
    }
}

QQmlListProperty<View> MainWindow::views()
{
    return {this, this,
        &MainWindow::appendView,
        &MainWindow::viewCount,
        &MainWindow::view,
        &MainWindow::clearViews,
        &MainWindow::replaceView,
        &MainWindow::removeLastView};
}

void MainWindow::appendView(View *view)
{
    this->m_views.append(view);
    emit this->viewsChanged();
}

qsizetype MainWindow::viewCount() const
{
    return this->m_views.count();
}

View *MainWindow::view(qsizetype index) const
{
    return this->m_views.at(index);
}

void MainWindow::clearViews()
{
    while(!this->m_views.isEmpty())
    {
        this->removeLastView();
    }
}

void MainWindow::replaceView(qsizetype index, View *view)
{
    this->m_views[index]->deleteLater();
    this->m_views[index] = view;
    emit this->viewsChanged();
}

void MainWindow::removeLastView()
{
    if(!this->m_views.isEmpty())
    {
        if(this->m_views.length() == this->m_activeViewIndex - 1)
        {
            this->setActiveViewIndex(this->m_activeViewIndex - 1);
        }

        this->m_views.takeLast()->deleteLater();
        emit this->viewsChanged();
    }
}

const QVector<View *> &MainWindow::getViews() const
{
    return this->m_views;
}

QVector<HistoryItem *> &MainWindow::getHistory()
{
    return this->m_history;
}

QQmlListProperty<HistoryItem> MainWindow::historyItems()
{
    return {this, this,
        &MainWindow::appendToHistoryItem,
        &MainWindow::historyItemsCount,
        &MainWindow::historyItem,
        &MainWindow::clearHistoryItems,
        &MainWindow::replaceHistoryItems,
        &MainWindow::removeLastHistoryItem};
}

void MainWindow::appendToHistoryItem(HistoryItem *historyItem)
{
    this->m_history.append(historyItem);
    emit this->historyItemsChanged();
}

qsizetype MainWindow::historyItemsCount() const
{
    return this->m_history.count();
}

HistoryItem *MainWindow::historyItem(qsizetype index) const
{
    return this->m_history.at(index);
}

void MainWindow::clearHistoryItems()
{
    while(!this->m_history.isEmpty())
    {
        this->removeLastView();
    }
}

void MainWindow::replaceHistoryItems(qsizetype index, HistoryItem *historyItem)
{
    this->m_history[index]->deleteLater();
    this->m_history[index] = historyItem;
    emit this->historyItemsChanged();
}

void MainWindow::removeLastHistoryItem()
{
    if(!this->m_history.isEmpty())
    {
        this->m_history.takeLast()->deleteLater();
        emit this->historyItemsChanged();
    }
}

void MainWindow::appendView(QQmlListProperty<View>* list, View* p) {
    reinterpret_cast< MainWindow* >(list->data)->appendView(p);
}

qsizetype MainWindow::viewCount(QQmlListProperty<View>* list) {
    return reinterpret_cast< MainWindow* >(list->data)->viewCount();
}

View* MainWindow::view(QQmlListProperty<View>* list, qsizetype i) {
    return reinterpret_cast< MainWindow* >(list->data)->view(i);
}

void MainWindow::clearViews(QQmlListProperty<View>* list) {
    reinterpret_cast< MainWindow* >(list->data)->clearViews();
}

void MainWindow::replaceView(QQmlListProperty<View> *list, qsizetype i, View *p)
{
    reinterpret_cast< MainWindow* >(list->data)->replaceView(i, p);
}

void MainWindow::removeLastView(QQmlListProperty<View> *list)
{
    reinterpret_cast< MainWindow* >(list->data)->removeLastView();
}

void MainWindow::appendToHistoryItem(QQmlListProperty<HistoryItem>* list, HistoryItem *historyItem) {
    reinterpret_cast< MainWindow* >(list->data)->appendToHistoryItem(historyItem);
}

qsizetype MainWindow::historyItemsCount(QQmlListProperty<HistoryItem>* list) {
    return reinterpret_cast< MainWindow* >(list->data)->historyItemsCount();
}

HistoryItem *MainWindow::historyItem(QQmlListProperty<HistoryItem>* list, qsizetype i) {
    return reinterpret_cast< MainWindow* >(list->data)->historyItem(i);
}

void MainWindow::clearHistoryItems(QQmlListProperty<HistoryItem>* list) {
    reinterpret_cast< MainWindow* >(list->data)->clearHistoryItems();
}

void MainWindow::replaceHistoryItems(QQmlListProperty<HistoryItem>* list, qsizetype i, HistoryItem *historyItem) {
    reinterpret_cast< MainWindow* >(list->data)->replaceHistoryItems(i, historyItem);
}

void MainWindow::removeLastHistoryItem(QQmlListProperty<HistoryItem>* list) {
    reinterpret_cast< MainWindow* >(list->data)->removeLastHistoryItem();
}


int MainWindow::getActiveViewIndex() const
{
    return this->m_activeViewIndex;
}

short MainWindow::getHistoryIndex() const
{
    return this->m_historyIndex;
}

void MainWindow::setHistoryIndex(short historyIndex)
{
    this->m_historyIndex = historyIndex;
    emit this->historyIndexChanged();
    this->m_window->getLocation()->setHref(this->historyItem(this->m_historyIndex)->getLocation()->getHref());
}

Auth *MainWindow::getAuth() const
{
    return this->m_auth;
}

bool MainWindow::get_OS_WASM() const
{
#ifdef Q_OS_WASM
    return true;
#else
    return false;
#endif
}

QUrl MainWindow::getThemeSource()
{
    return this->m_themeSource;
}

void MainWindow::setThemeSource(QUrl themeSource)
{
    this->m_themeSource = themeSource;
    this->getThemeComponent()->loadUrl(themeSource);

    emit this->themeSourceChanged();

    // Emit themeComponentChanged because if the source changes then qml documents using the theme component need
    // to know to update
    emit this->themeComponentChanged();
}

QUrl MainWindow::getHomePageUrl()
{
    return this->m_homePageUrl;
}

void MainWindow::setHomePageUrl(QUrl homePageUrl)
{
    this->m_homePageUrl = homePageUrl;
    emit this->homePageUrlChanged();
}

void MainWindow::resetThemeComponent()
{
    QQmlComponent *oldThemeComponent = this->getThemeComponent();
    this->m_themeComponent = nullptr;
    oldThemeComponent->deleteLater();
    emit this->themeComponentChanged();
}

QQmlComponent *MainWindow::getThemeComponent()
{
    if (this->m_themeComponent == nullptr)
    {
        this->m_themeComponent = new QQmlComponent(this->getQmlEngine(), QUrl{this->getThemeSource()});
    }
    return this->m_themeComponent;
}

void MainWindow::setThemeComponent(QQmlComponent *themeComponent)
{
    this->m_themeComponent = themeComponent;
    emit this->themeComponentChanged();
}

QString MainWindow::getBuild() const
{
    return CANONIC_BUILD;
}

void MainWindow::setBuild(QString build)
{
#ifdef Q_OS_WASM
    EM_ASM(
        {
            var localStorage = window.localStorage;
            localStorage.setItem('canonic-build', UTF8ToString($0));
        },
        build.toStdString().c_str()
    );
#endif
}

void MainWindow::mainUILoaded() const
{
#ifdef Q_OS_WASM
    EM_ASM(
        if(window.canonic === undefined)
        {
            console.error("window.canonic is undefined. Are you running in a dev enironment?")
        }
        else {
            window.canonic.__hideSpinner()
        }
    );
#endif
}

QQmlEngine* MainWindow::getQmlEngine() const
{
    return this->m_hostViewport->getQmlEngine();
}

void MainWindow::refresh()
{
    this->m_contentViewport->reloadTLISource();
    /*
    if(this->m_rootItem != nullptr)
    {
        QUrl tmp = m_rootItem->property("source").toString();
        m_rootItem->setProperty("source", QUrl(""));
        this->m_qmlEngine->clearComponentCache();
        this->resetThemeComponent();
        m_rootItem->setProperty("source", tmp);
    }
    */
}

void MainWindow::setActiveViewIndex(int activeViewIndex)
{
    std::cout << "setActiveViewIndex: " << activeViewIndex << std::endl;
    this->m_activeViewIndex = activeViewIndex;
    emit this->activeViewIndexChanged(activeViewIndex);

    /*
    if(activeViewIndex >= 0 && activeViewIndex < this->viewCount())
    {
        this->m_contentViewport->setTLISource(this->view(activeViewIndex)->getQmlSource());
    }
    else {
        this->m_contentViewport->setTLISource(QUrl(""));
    }
    */
}

View *MainWindow::getActiveView() const
{
    if(this->m_activeViewIndex >= 0 && m_activeViewIndex < this->viewCount())
    {
        return this->view(this->m_activeViewIndex);
    }
    return nullptr;
}

void MainWindow::updateGlobalHistory(QString href)
{
    std::cout << "m_shouldIncrementHistoryIndex:" << this->m_shouldIncrementHistoryIndex << std::endl;
    if(this->m_shouldIncrementHistoryIndex){

        if(!this->m_history.isEmpty())
        {
            // Don't add to history if it's the same location
            //if(this->m_history.last()->getLocation()->getHref() == href)
            if(this->historyItem(this->m_historyIndex)->getLocation()->getHref() == href)
            {
                return;
            }
        }

        std::cout << "incremneting history index:" << std::endl;
        this->m_historyIndex++;

        // Remove any forward pages
        while(this->m_history.count() > this->m_historyIndex)
        {
            std::cout << "removing:" << this->historyItem(this->historyItemsCount() - 1)->getLocation()->getHref().toStdString() << "  " << this->historyItemsCount() << std::endl;
            this->removeLastHistoryItem();
        }

        this->appendToHistoryItem(new HistoryItem(href));

        // Emit index change event after appending the new history item
        emit this->historyIndexChanged();
    }

    this->m_shouldIncrementHistoryIndex = true;
}

void MainWindow::refreshMainUI()
{
    this->refresh();
}
