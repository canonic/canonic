#include "../include/MainWindow.hpp"
#include "../include/Location.hpp"
#include "../include/MainWindow.hpp"
#include "../include/View.hpp"
#include "../include/Window.hpp"

#include "../include/CxxSyntaxHighlighter.hpp"

#ifdef Q_OS_WASM
#include <emscripten.h>
#endif

#include <QQmlEngine>
#include <QtQml>

#include <iostream>

MainWindow::MainWindow()
{
    this->m_auth = new Auth();
    this->m_networkAccessManagerFactory = new NetworkAccessManagerFactory(this);
    this->m_window = new WebAPI::Window(this);
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

void MainWindow::setActiveViewIndex(int activeViewIndex)
{
    std::cout << "setActiveViewIndex: " << activeViewIndex << std::endl;
    this->m_activeViewIndex = activeViewIndex;
    emit this->activeViewIndexChanged(this->m_activeViewIndex);
}

void MainWindow::handleMouseDoubleClickEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
}

void MainWindow::handlePositionChangedEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
}

void MainWindow::handlePressEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
}

void MainWindow::handleReleaseEvent(QPointF localPos,
    int button,
    int buttons,
    int modifiers,
    int source)
{
}

void MainWindow::handleWheelEvent(QPointF localPos,
    QPoint pixelDelta,
    QPoint angleDelta,
    int buttons,
    int modifiers,
    bool inverted,
    int source)
{
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
