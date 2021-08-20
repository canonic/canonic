#pragma once

#include <QNetworkAccessManager>
#include <QQmlNetworkAccessManagerFactory>
#include "MainWindow.fwd.hpp"
#include "ProxyFactory.hpp"

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
  public:
    NetworkAccessManagerFactory(MainWindow *mainWindow);
    QNetworkAccessManager *create(QObject *parent) override;

  private:
    MainWindow *m_mainWindow;
    ProxyFactory *m_proxyFactory;
};
