#pragma once

#include <QNetworkProxyFactory>

class ProxyFactory : public QNetworkProxyFactory
{
  public:
    QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query) override;
};
