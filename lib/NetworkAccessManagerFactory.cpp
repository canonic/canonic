#include "../include/NetworkAccessManagerFactory.hpp"
#include "../include/NetworkAccessManager.hpp"
#include "../include/MainWindow.hpp"
#include <iostream>

NetworkAccessManagerFactory::NetworkAccessManagerFactory(MainWindow *mainWindow): m_mainWindow{mainWindow},
                                                                                  m_proxyFactory{new ProxyFactory}
{
}

QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *parent)
{
    NetworkAccessManager *nam = new NetworkAccessManager(this->m_mainWindow, parent);
    nam->setProxyFactory(this->m_proxyFactory);
    return nam;
}
