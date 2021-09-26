#include <QtQml>
#include <QDesktopServices>

#include "../include/Window.hpp"
#include "../include/QMLView.hpp"

#include <iostream>


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
                    QDesktopServices::openUrl(url);
                }
            }
        }
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

        std::cout << "handleLocationHrefChange: " << href.toStdString() << std::endl;
        std::cout << "hardReload: " << hardReload << std::endl;
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

        std::cout << "got reply" << std::endl;

        // Clear the current views
        std::cout << "clear views" << std::endl;
        this->m_mainWindow->clearViews();

        const QByteArray& rawData = reply->readAll();
        int activeViewIndex = 0;
        QJsonObject objectType;
        QJsonObject objectValue;


        if(reply->error())
        {
            std::cout << "reply has an error" << std::endl;
            std::cout << reply->errorString().toStdString() << std::endl;
            /*
            View *networkErrorView = new View{"Network Error View", QUrl{""}, QUrl{"qrc:/qml/NetworkErrorView.qml"}};
            this->m_mainWindow->appendView(networkErrorView);
            */
        }
        else {
            /*
            // Debug view is always supported
            View *debugView = new View{"Debug View", QUrl{""}, QUrl{"qrc:/qml/DebugView.qml"}};
            this->m_mainWindow->appendView(debugView);

            // Raw source view is always supported
            View *rawSourceView = new View{"Raw Source View", QUrl{""}, QUrl{"qrc:/qml/RawSourceView.qml"}};
            this->m_mainWindow->appendView(rawSourceView);
            std::cout << "added raw source" << std::endl;

            // Default to raw source view
            activeViewIndex = 1;

            QJsonParseError jsonError;
            QJsonDocument jsonDocument = QJsonDocument::fromJson( rawData, &jsonError );
            if( jsonError.error == QJsonParseError::NoError )
            {
                View *jsonFileView = new View{"JSON File View", QUrl{""}, QUrl{"qrc:/qml/JSONFileView.qml"}};
                this->m_mainWindow->appendView(jsonFileView);
                activeViewIndex++;

                if( jsonDocument.isObject() )
                {
                    QJsonObject jsonObj = jsonDocument.object();

                    if(jsonObj.contains("type") && jsonObj.contains("value") && jsonObj.contains("views"))
                    {
                        QJsonValue jsonType = jsonObj.value("type");
                        QJsonValue jsonValue = jsonObj.value("value");

                        if(jsonValue.isObject())
                        {
                            objectValue = jsonValue.toObject();

                            if(jsonType.isString())
                            {
                                QString type = jsonType.toString();
                                if(type == "VARDECL_TYPE")
                                {
                                    View *varDeclListView = new View{"VarDecl List View", QUrl{""}, QUrl{"https://www.canonic.com/klata-ui/release/KlataUI/VarDeclListView.qml"}};
                                    this->m_mainWindow->appendView(varDeclListView);
                                    activeViewIndex++;
                                }
                                else if(type == "FUNCTIONDECL_TYPE")
                                {
                                    View *varDeclListView = new View{"FunctionDecl List View", QUrl{""}, QUrl{"https://www.canonic.com/klata-ui/release/KlataUI/FunctionDeclListView.qml"}};
                                    this->m_mainWindow->appendView(varDeclListView);
                                    activeViewIndex++;
                                }
                                else if(type == "KLATA_REPO")
                                {
                                    View *klatahRepoView = new View{"Klatah Repo View", QUrl{""}, QUrl{"https://www.canonic.com/klata-ui/release/KlataUI/KlatahRepoView.qml"}};
                                    this->m_mainWindow->appendView(klatahRepoView);
                                    activeViewIndex++;
                                }
                            }
                        }
                    }
                }
            }
            */

            QVariant contentType = reply->header(QNetworkRequest::ContentTypeHeader);

            /*
            if (contentType.isValid() && contentType.toString().contains("text/html"))
            {
                View *htmlDocumentView = new View{"HTML Document View", QUrl{""}, QUrl{"qrc:/qml/HTMLDocumentView.qml"}};
                this->m_mainWindow->appendView(htmlDocumentView);
                activeViewIndex++;
            }
            */

            if(reply->url().path().endsWith(".qml", Qt::CaseInsensitive) ||
                (contentType.isValid() && contentType.toString().contains("text/qml")))
            {
                View *qmlDocumentView = new QMLView{};
                this->m_mainWindow->appendView(qmlDocumentView);
                //activeViewIndex++;
            }
        }

        this->m_document->setRawData(rawData);
        this->m_document->setObjectType(objectType);
        this->m_document->setObjectValue(objectValue);
        this->m_mainWindow->setActiveViewIndex(activeViewIndex);
        this->m_mainWindow->updateGlobalHistory(this->m_location->getHref());

        /*
        QFile file("://qml/TLI.qml");
        if (file.open(QIODevice::ReadOnly))
        {
            this->m_mainWindow->m_contentViewport->setData(file.readAll(), QUrl(QStringLiteral("qrc:/qml/TLI.qml")));
            file.close();
        }
        */

        reply->deleteLater();
        std::cout << "Finished Request" << std::endl;
    }
}


