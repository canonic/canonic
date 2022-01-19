#include "../include/NetworkAccessManager.hpp"
#include "../include/MainWindow.hpp"
#include <iostream>

NetworkAccessManager::NetworkAccessManager(MainWindow *mainWindow,
    QObject *parent): QNetworkAccessManager{parent},
                       m_mainWindow{mainWindow}
{

}

QNetworkReply *NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op,
    const QNetworkRequest &originalReq,
    QIODevice *outgoingData)
{
    QNetworkRequest networkRequest{originalReq};
    QString url = networkRequest.url().toString();

    if(this->m_mainWindow->getAuth()->isUserSignedIn())
    {
        networkRequest.setRawHeader(QByteArray("Proof-Of-Identity"),
            this->m_mainWindow->getAuth()->getProofOfIdentity().toLatin1());

        networkRequest.setRawHeader(QByteArray("Proof-Of-Identity-Type"),
            this->m_mainWindow->getAuth()->getProofOfIdentityType().toLatin1());
    }

    // Send empty urls to the browsers homepage
    if(url.length() == 0)
    {
        networkRequest.setUrl(this->m_mainWindow->getHomePageUrl());
        url = networkRequest.url().toString();
    }

    // Send www.canonic.com to the QML home page
    if(url == "https://www.canonic.com/")
    {
        networkRequest.setUrl(QUrl("https://www.canonic.com/main.qml"));
        url = networkRequest.url().toString();
    }

    // So that http servers have a way to differentiate HTML focused browsers vs
    // QML focused browsers set 'text/qml' as the primary accepted content type.
    QByteArray originalAcceptHeader = originalReq.rawHeader("Accept");
    if (originalAcceptHeader == "" or originalAcceptHeader == "*/*")
    {
        networkRequest.setRawHeader("Accept", "text/qml;q=1.0, */*;q=0.9");
    }

#ifdef Q_OS_WASM
    // Proxy http and https requests to avoid CORS issues
    if(networkRequest.url().scheme().contains("http"))
    {
        if(!(url.startsWith("https://app.canonic.com") ||
             url.startsWith("https://playground.canonic.com") ||
             url.startsWith("https://www.canonic.com")))
        {
            networkRequest.setUrl(QUrl("https://app.canonic.com/proxy/" + networkRequest.url().toString()));
            networkRequest.setRawHeader(QByteArray("x-requested-with"), "canonic");
        }
    }
#endif

    return QNetworkAccessManager::createRequest(op, networkRequest, outgoingData);
}
