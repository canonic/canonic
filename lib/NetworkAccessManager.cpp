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

    if(url.length() == 0)
    {
        networkRequest.setUrl(this->m_mainWindow->getHomePageUrl());
    }

    /*
    if(url.startsWith("https://www.canonic.com/comparision/renderfarms"))
    {
        networkRequest.setUrl(QUrl(url.replace("https://www.canonic.com/comparision/renderfarms", "file:///S:/canonic/renderfarms")));
    }

    QString metonymUrl = "https://www.canonic.com/metonym/release/";
    if(url.startsWith(metonymUrl))
    {
        networkRequest.setUrl(QUrl(url.replace(metonymUrl, "file:///S:/canonic/metonym/web/release/")));
    }

    QString klataUIUrl = "https://www.canonic.com/klata-ui/release/KlataUI/";
    if(url.startsWith(klataUIUrl))
    {
        networkRequest.setUrl(QUrl(url.replace(klataUIUrl, "file:///S:/canonic/klata-ui/web/release/KlataUI/")));
    }

    if(url.startsWith("https://raw.githubusercontent.com/canonic/canonic-qml-web-directory/main"))
    {
        networkRequest.setUrl(QUrl(url.replace("https://raw.githubusercontent.com/canonic/canonic-qml-web-directory/main", "file:///S:/canonic/canonic-qml-web-directory")));
    }*/

#ifdef Q_OS_WASM
    if(networkRequest.url().scheme().contains("http"))
    {
        networkRequest.setUrl(QUrl("https://proxy.canonic.com/proxy/" + networkRequest.url().toString()));
        networkRequest.setRawHeader(QByteArray("x-requested-with"), "canonic");
    }
#endif

    return QNetworkAccessManager::createRequest(op, networkRequest, outgoingData);
}
