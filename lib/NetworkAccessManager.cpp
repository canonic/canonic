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

#ifdef Q_OS_WASM
    if(networkRequest.url().scheme().contains("http"))
    {
        networkRequest.setUrl(QUrl("https://proxy.canonic.com/proxy/" + networkRequest.url().toString()));
        networkRequest.setRawHeader(QByteArray("x-requested-with"), "canonic");
    }
#endif

    return QNetworkAccessManager::createRequest(op, networkRequest, outgoingData);
}
