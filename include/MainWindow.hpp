#pragma once

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QQuickItem>
#include <QResizeEvent>
#include <QTimer>
#include <QVector>
#include <QWindow>

#include "./Auth.hpp"
#include "./CanonicRenderer.hpp"
#include "./ContentViewport.hpp"
#include "./HistoryItem.hpp"
#include "./HostViewport.hpp"
#include "./NetworkAccessManagerFactory.hpp"
#include "./RenderControl.hpp"
#include "./Window.fwd.hpp"
#include "./View.hpp"
#include "./Location.hpp"

class MainWindow: public QWindow
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<View> views READ views NOTIFY viewsChanged)
    Q_PROPERTY(QQmlListProperty<HistoryItem> historyItems READ historyItems NOTIFY historyItemsChanged)
    Q_PROPERTY(short historyItemIndex READ getHistoryIndex WRITE setHistoryIndex NOTIFY historyIndexChanged)
    Q_PROPERTY(int activeViewIndex READ getActiveViewIndex WRITE setActiveViewIndex NOTIFY activeViewIndexChanged)
    Q_PROPERTY(Auth *auth READ getAuth CONSTANT)
    Q_PROPERTY(bool OS_WASM READ get_OS_WASM CONSTANT)
    Q_PROPERTY(QUrl themeSource READ getThemeSource WRITE setThemeSource NOTIFY themeSourceChanged)
    Q_PROPERTY(QUrl homePage READ getHomePageUrl WRITE setHomePageUrl NOTIFY homePageUrlChanged)

    // todo create a separate theme component for the virtualised qmlengines (when we add them)
    Q_PROPERTY(QQmlComponent *themeComponent READ getThemeComponent WRITE setThemeComponent NOTIFY themeComponentChanged)
    Q_PROPERTY(QString build READ getBuild WRITE setBuild NOTIFY buildChanged)

  signals:
    void viewsChanged();
    void historyItemsChanged();
    void historyIndexChanged();
    void activeViewIndexChanged(int activeViewIndex);
    void themeSourceChanged();
    void themeComponentChanged();
    void homePageUrlChanged();
    void buildChanged();

  public:
    MainWindow();
    ~MainWindow();

    QQmlListProperty<View> views();
    void appendView(View *view);
    qsizetype viewCount() const;
    View *view(qsizetype index) const;
    void clearViews();
    void replaceView(qsizetype index, View *view);
    void removeLastView();
    const QVector<View *> &getViews() const;

    int getActiveViewIndex() const;
    void setActiveViewIndex(int activeViewIndex);

    View *getActiveView() const;

    QVector<HistoryItem *> &getHistory();

    QQmlListProperty<HistoryItem> historyItems();
    void appendToHistoryItem(HistoryItem *historyItem);
    qsizetype historyItemsCount() const;
    HistoryItem *historyItem(qsizetype index) const;
    void clearHistoryItems();
    void replaceHistoryItems(qsizetype index, HistoryItem *historyItem);
    void removeLastHistoryItem();

    short getHistoryIndex() const;
    void setHistoryIndex(short historyIndex);

    Auth *getAuth() const;
    bool get_OS_WASM() const;

    Q_INVOKABLE
    QUrl getThemeSource();

    Q_INVOKABLE
    void setThemeSource(QUrl themeSource);

    Q_INVOKABLE
    QUrl getHomePageUrl();

    Q_INVOKABLE
    void setHomePageUrl(QUrl homePageUrl);

    Q_INVOKABLE
    QQmlComponent *getThemeComponent();

    Q_INVOKABLE
    void setThemeComponent(QQmlComponent *themeComponent);

    Q_INVOKABLE
    void resetThemeComponent();

    QString getBuild() const;
    void setBuild(QString build);

    QQmlEngine* getQmlEngine() const;
    void resetContentViewport();

    /**
     * Used to hide the loading spinner in wasm distributions.
     */
    Q_INVOKABLE
    void mainUILoaded() const;

    Q_INVOKABLE
    void previousPage();

    Q_INVOKABLE
    void nextPage();

  //protected:
    static void appendView(QQmlListProperty<View>*, View*);
    static qsizetype viewCount(QQmlListProperty<View>*);
    static View* view(QQmlListProperty<View>*, qsizetype);
    static void clearViews(QQmlListProperty<View>*);
    static void replaceView(QQmlListProperty<View>*, qsizetype, View*);
    static void removeLastView(QQmlListProperty<View>*);

    static void appendToHistoryItem(QQmlListProperty<HistoryItem>*, HistoryItem*);
    static qsizetype historyItemsCount(QQmlListProperty<HistoryItem>*);
    static HistoryItem* historyItem(QQmlListProperty<HistoryItem>*, qsizetype);
    static void clearHistoryItems(QQmlListProperty<HistoryItem>*);
    static void replaceHistoryItems(QQmlListProperty<HistoryItem>*, qsizetype, HistoryItem*);
    static void removeLastHistoryItem(QQmlListProperty<HistoryItem>*);

    NetworkAccessManagerFactory* m_networkAccessManagerFactory;
    WebAPI::Window *m_window;
    QVector<View *> m_views;
    int m_activeViewIndex;
    Auth *m_auth;
    bool m_isLoading{false};
    bool m_shouldIncrementHistoryIndex{true};
    short m_historyIndex{-1};
    QVector<HistoryItem *> m_history;
    QUrl m_themeSource{"https://www.canonic.com/metonym/release/Metonym/CanonicDarkTheme.qml"};
    QUrl m_homePageUrl{"https://raw.githubusercontent.com/canonic/canonic-qml-web-directory/main/main.qml"};
    QQmlComponent *m_themeComponent{nullptr};

    QOpenGLContext *m_context;
    QOffscreenSurface *m_offscreenSurface;
    ContentViewport *m_contentViewport;
    HostViewport *m_hostViewport;
    QTimer m_updateTimer;
    CanonicRenderer *m_canonicRenderer;

  protected:
    void exposeEvent(QExposeEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    bool event(QEvent *e) override;

  private:
    QRectF getNormalisedViewportGeometry() const;

  public slots:
    void updateGlobalHistory(QString href);

    void handleHostViewportStatusChange(Viewport::Status status);
    void handleContentViewportStatusChange(Viewport::Status status);

  private slots:
    void render();
    void requestUpdate();
    void handleScreenChange();

};
