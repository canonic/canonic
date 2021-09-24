#pragma once

#include <QNetworkProxyFactory>

/**
 * Does nothing for now given Qt's poor proxy support for WASM
 **/
class ProxyFactory : public QNetworkProxyFactory
{
  public:
    QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query) override;
};
