#pragma once

#include <QQuickItem>
#include <QResizeEvent>
#include <QVector>

#include "./Auth.hpp"
#include "./HistoryItem.hpp"
#include "./NetworkAccessManagerFactory.hpp"
#include "./Window.fwd.hpp"
#include "./View.hpp"
#include "./Location.hpp"

class MainWindow: public QObject
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
    void onResize(QResizeEvent* event);
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

    virtual const int width() const = 0;
    virtual const int height() const = 0;
    virtual void resize(int w, int h) = 0;
    virtual void setGeometry(int x, int y, int w, int h) = 0;
    virtual void show() = 0;

    QQmlListProperty<View> views();
    void appendView(View *view);
    qsizetype viewCount() const;
    View *view(qsizetype index) const;
    void clearViews();
    void replaceView(qsizetype index, View *view);
    void removeLastView();
    const QVector<View *> &getViews() const;

    int getActiveViewIndex() const;
    virtual void setActiveViewIndex(int activeViewIndex);

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

    virtual QQmlEngine* getQmlEngine() const = 0;
    virtual void refresh() = 0;

    Q_INVOKABLE
    void mainUILoaded() const;

    Q_INVOKABLE
    virtual void handleMouseDoubleClickEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    Q_INVOKABLE
    virtual void handlePositionChangedEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    Q_INVOKABLE
    virtual void handlePressEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    Q_INVOKABLE
    virtual void handleReleaseEvent(QPointF localPos,
        int button,
        int buttons,
        int modifiers,
        int source);

    Q_INVOKABLE
    virtual void handleWheelEvent(QPointF localPos,
        QPoint pixelDelta,
        QPoint angleDelta,
        int buttons,
        int modifiers,
        bool inverted,
        int source);

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

  public slots:
    void updateGlobalHistory(QString href);
    void refreshMainUI();

};

Q_DECLARE_INTERFACE(MainWindow, "com.mycompany.qmlcomponents/1.0")
