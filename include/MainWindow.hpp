#pragma once

#include <QNetworkReply>
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
    Q_PROPERTY(QUrl homePage READ getHomePageUrl WRITE setHomePageUrl NOTIFY homePageUrlChanged)
    Q_PROPERTY(QString build READ getBuild WRITE setBuild NOTIFY buildChanged)
    Q_PROPERTY(QString buildDate READ getBuildDate CONSTANT)
    Q_PROPERTY(QString gitHash READ getGitHash CONSTANT)
    Q_PROPERTY(QString gitBranch READ getGitBranch CONSTANT)
    Q_PROPERTY(QString theme READ getTheme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QNetworkReply::NetworkError networkReplyError READ getNetworkReplyError NOTIFY networkReplyErrorChanged)
    Q_PROPERTY(QString networkReplyErrorString READ getNetworkReplyErrorString NOTIFY networkReplyErrorStringChanged)
    Q_PROPERTY(qint64 uploadProgress READ getUploadProgress NOTIFY uploadProgressChanged)
    Q_PROPERTY(qint64 downloadProgress READ getDownloadProgress NOTIFY downloadProgressChanged)
    Q_PROPERTY(Viewport::Status contentViewportStatus READ getContentViewportStatus NOTIFY contentViewportStatusChanged)
    Q_PROPERTY(QString contentViewportErrorString READ getContentViewportErrorString NOTIFY contentViewportErrorStringChanged)

  signals:
    void viewsChanged();
    void historyItemsChanged();
    void historyIndexChanged();
    void activeViewIndexChanged(int activeViewIndex);
    void homePageUrlChanged();
    void buildChanged();
    void themeChanged();
    void networkReplyErrorChanged(QNetworkReply::NetworkError error);
    void networkReplyErrorStringChanged(QString errorString);
    void uploadProgressChanged(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);
    void contentViewportStatusChanged();
    void contentViewportErrorStringChanged();

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
    QUrl getHomePageUrl();

    Q_INVOKABLE
    void setHomePageUrl(QUrl homePageUrl);

    QString getBuild() const;
    void setBuild(QString build);

    const char *getBuildDate() const;
    const char *getGitHash() const;
    const char *getGitBranch() const;

    QQmlEngine* getQmlEngine() const;
    void resetContentViewport();

    QString getTheme() const;
    void setTheme(QString theme);

    QNetworkReply::NetworkError getNetworkReplyError() const;
    void setNetworkReplyError(QNetworkReply::NetworkError error, QString errorString = "");
    QString getNetworkReplyErrorString() const;

    qint64 getUploadProgress() const;
    void setUploadProgress(qint64 bytesSent, qint64 bytesTotal);

    qint64 getDownloadProgress() const;
    void setDownloadProgress(qint64 bytesRecieved, qint64 bytesTotal);

    Viewport::Status getContentViewportStatus() const;
    QString getContentViewportErrorString() const;

    /**
     * Used to hide the loading spinner in wasm distributions.
     */
    Q_INVOKABLE
    void mainUILoaded();

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
    QUrl m_homePageUrl{"https://raw.githubusercontent.com/canonic/canonic-qml-web-directory/main/main.qml"};
    QString m_theme;
    qint64 m_uploadProgress;
    qint64 m_downloadProgress;

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
    bool m_mainUILoaded;
    QNetworkReply::NetworkError m_networkReplyError;
    QString m_networkReplyErrorString;

  public slots:
    void updateGlobalHistory(QString href);

    void handleHostViewportStatusChange(Viewport::Status status);
    void handleContentViewportStatusChange(Viewport::Status status);

  private slots:
    void render();
    void requestUpdate();
    void handleScreenChange();

};
