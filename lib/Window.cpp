#include <QtQml>
#include <QDesktopServices>

#include "../include/Window.hpp"
#include "../include/DebugView.hpp"
#include "../include/HTMLView.hpp"
#include "../include/JSONView.hpp"
#include "../include/QMLView.hpp"
#include "../include/RawSourceView.hpp"

#ifdef Q_OS_WASM
#include <emscripten.h>

EM_JS(void, pushHistoryState, (const char *url, int urlLength), {
    var origin = window.location.origin + '/';
    var canonicUrl = UTF8ToString(url, urlLength);
    var newUrl;
    var title;

    // Cache the original page title
    if (window.__original_title === undefined)
    {
        window.__original_title = document.title
    }

    // The home page is a special case where we do not want to append a fragment
    // or modify the title
    if (origin === canonicUrl)
    {
        newUrl = origin;
        title = window.__original_title;
    }
    else
    {
        newUrl = origin + '#' + canonicUrl;
        title = 'Canonic (' + canonicUrl + ')';
    }

    if (newUrl !== window.location.href)
    {
        window.history.pushState({}, title, newUrl);
        document.title = title;
    }
});
#endif

namespace WebAPI {
    Window::Window(MainWindow *mainWindow, QObject *parent)
        : QObject(parent),
          m_document{new Document(mainWindow)},
          m_location{new Location()},
          m_navigator{new Navigator()},
          m_mainWindow{mainWindow}
    {
        QObject::connect(this->m_location, &Location::requiresReload,
            this, &Window::handleLocationHrefChange);

        QObject::connect(this->m_mainWindow, &MainWindow::activeViewIndexChanged,
            this, &Window::viewSourceChanged);

        QObject::connect(this->m_mainWindow, &MainWindow::themeChanged,
            this, &Window::themeChanged);
    }

    Location *Window::getLocation() const {
        return m_location;
    }

    void Window::setLocation(Location *location) {
        m_location = location;
        emit locationChanged(m_location);
    }

    Navigator *Window::getNavigator() const {
        return m_navigator;
    }

    int Window::getInnerScreenX() const
    {
        return this->getOuterWidth() - this->getInnerWidth();
    }

    int Window::getInnerScreenY() const
    {
        return this->getOuterHeight() - this->getInnerHeight();
    }

    int Window::getInnerWidth() const
    {
        return this->m_mainWindow->width();
    }

    int Window::getInnerHeight() const
    {
        return this->m_mainWindow->height() - 44;
    }

    int Window::getOuterWidth() const
    {
        return this->m_mainWindow->width();
    }

    int Window::getOuterHeight() const
    {
        return this->m_mainWindow->height();
    }

    Document *Window::getDocument() const
    {
        return this->m_document;
    }

    QString Window::getTheme() const
    {
        return this->m_mainWindow->getTheme();
    }

    QString Window::btoa(QString str) const
    {
        QByteArray::FromBase64Result res = QByteArray::fromBase64Encoding(str.toLatin1());

        if(res.decodingStatus != QByteArray::Base64DecodingStatus::Ok)
        {
            return QString("");
        }

        return QString::fromLatin1(res.decoded);
    }

    QString Window::atob(QString str) const
    {
        return str.toLatin1().toBase64();
    }

    void Window::open(QString url, QString windowName, QString windowFeatures)
    {
        QStringList windowFeaturesSplit = windowFeatures.split(QLatin1Char(','), Qt::SkipEmptyParts);

        for (const QString &windowFeature : windowFeaturesSplit)
        {
            QStringList windowsFeatureSplit = windowFeature.split(QLatin1Char('='), Qt::SkipEmptyParts);

            if (windowsFeatureSplit.length() == 2)
            {
                QString key = windowsFeatureSplit[0];
                QString value = windowsFeatureSplit[1];

                if (key == "external" && value == "yes")
                {
                    // Open the url in the users default browser
                    QDesktopServices::openUrl(url);
                    return;
                }
            }
        }

        this->getLocation()->setHref(url);
    }

    void Window::handleWindowResize()
    {
        emit innerScreenXChanged(this->getInnerScreenX());
        emit innerScreenYChanged(this->getInnerScreenY());
        emit innerWidthChanged(this->getInnerWidth());
        emit innerHeightChanged(this->getInnerHeight());
        emit outerWidthChanged(this->getOuterWidth());
        emit outerHeightChanged(this->getOuterHeight());
    }

    void Window::handleLocationHrefChange(QString href, bool hardReload)
    {
        if(this->m_networkReply && this->m_networkReply->isRunning())
        {
            this->m_networkReply->abort();
            this->m_networkReply = nullptr;
        }

        QNetworkRequest request{QUrl{href}};

        this->m_mainWindow->resetContentViewport();
        QQmlEngine *qQmlEngine = this->m_mainWindow->getQmlEngine();
        QNetworkAccessManager *nam = qQmlEngine->networkAccessManager();

        this->m_networkReply = nam->get(request);

        WebAPI::Document *previousDoc = this->m_document;

        // Create a new Document object
        this->m_document = new WebAPI::Document(this->m_mainWindow);
        this->m_document->setURL(href);
        this->m_document->setReadyState("loading");
        emit this->documentChanged(this->m_document);

        // Delete the old document object
        previousDoc->deleteLater();

        // reset upload and download progress
        this->m_mainWindow->setNetworkReplyError(QNetworkReply::NetworkError::NoError);
        this->m_mainWindow->setUploadProgress(0, -1);
        this->m_mainWindow->setDownloadProgress(0, -1);

        this->connect(this->m_networkReply, &QNetworkReply::uploadProgress,
            this->m_mainWindow, &MainWindow::setUploadProgress);

        this->connect(this->m_networkReply, &QNetworkReply::downloadProgress,
            this->m_mainWindow, &MainWindow::setDownloadProgress);

        this->connect(this->m_networkReply, &QNetworkReply::finished,
            this, &Window::handleFinishedLoadingReply);

    }

    void Window::handleFinishedLoadingReply()
    {
        this->m_document->setReadyState("complete");

        QNetworkReply *reply = (QNetworkReply *)this->sender();

        if(this->m_networkReply == reply)
        {
            this->m_networkReply = nullptr;
        }

        // Clear the current views
        this->m_mainWindow->clearViews();

        if(reply->error())
        {
            this->m_mainWindow->setNetworkReplyError(reply->error(), reply->errorString());
        }
        else {
            const QByteArray& rawData = reply->readAll();

            int activeViewIndex = 0;
            QJsonObject objectType;
            QJsonObject objectValue;

            // Debug view is always supported
            this->m_mainWindow->appendView(new DebugView{});

            this->m_mainWindow->appendView(new RawSourceView{});
            activeViewIndex++;

            QJsonParseError jsonError;
            QJsonDocument jsonDocument = QJsonDocument::fromJson( rawData, &jsonError );
            if( jsonError.error == QJsonParseError::NoError )
            {
                this->m_mainWindow->appendView(new JSONView{});
                activeViewIndex++;
            }

            QVariant contentType = reply->header(QNetworkRequest::ContentTypeHeader);

            if (contentType.isValid() && contentType.toString().contains("text/html"))
            {
                View *htmlDocumentView = new HTMLView{};
                this->m_mainWindow->appendView(htmlDocumentView);
                activeViewIndex++;
            }

            if(reply->url().path().endsWith(".qml", Qt::CaseInsensitive) ||
                (contentType.isValid() && contentType.toString().contains("text/qml")))
            {
                View *qmlDocumentView = new QMLView{};
                this->m_mainWindow->appendView(qmlDocumentView);
                activeViewIndex++;
            }

            this->m_document->setRawData(rawData);
            this->m_document->setObjectType(objectType);
            this->m_document->setObjectValue(objectValue);
            this->m_mainWindow->setActiveViewIndex(activeViewIndex);
            this->m_mainWindow->updateGlobalHistory(this->m_location->getHref());

#ifdef Q_OS_WASM
            std::string urlString = this->m_location->getHref().toStdString();
            pushHistoryState(urlString.c_str(), urlString.length());
#endif
        }

        reply->deleteLater();
    }
}


