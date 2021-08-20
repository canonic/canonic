#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "MainWindow.fwd.hpp"

class NetworkAccessManager : public QNetworkAccessManager
{
  public:
    NetworkAccessManager(MainWindow *mainWindow, QObject *parent);

  protected:
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op,
        const QNetworkRequest &originalReq,
        QIODevice *outgoingData = nullptr) override;

  private:
    MainWindow *m_mainWindow;
};
