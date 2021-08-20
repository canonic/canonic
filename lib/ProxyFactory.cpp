#include "../include/ProxyFactory.hpp"
#include <iostream>

QList<QNetworkProxy> ProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
    /* Qt webassembly proxy support seems to be non-existant for now */
    return { QNetworkProxy::NoProxy };
}
