#include <QtQml>
#include <QUrl>

#include "../include/Location.hpp"

namespace WebAPI {
    Location::Location(QObject *parent) : QObject(parent), m_internal{""}
    {
        //QObject::connect(this, &Location::hrefChanged, this, &Location::reload);
        QObject::connect(this, SIGNAL(hrefChanged(QString)), this, SLOT(reload()));
    }

    Location::Location(QString location, QObject *parent) : QObject(parent)
    {
        this->m_internal = QUrl(location);
    }

    QString Location::getAncestorOrigins() const
    {
        // QML does not (as far as I know) have an analogous type to html's iframe so it should always
        // return undefined. Not sure how to return undefined though from c++
        return "";
    }

    QString Location::getHref() const
    {
        return this->m_internal.toString();
    }

    void Location::setHref(QString href)
    {
        this->m_internal = QUrl(href);
        emit ancestorOriginsChanged(this->getAncestorOrigins());
        emit hrefChanged(href);
        emit protocolChanged(this->getProtocol());
        emit hostChanged(this->getHost());
        emit hostnameChanged(this->getHostname());
        emit portChanged(this->getPort());
        emit pathnameChanged(this->getPathname());
        emit searchChanged(this->getSearch());
        emit hashChanged(this->getHash());
        emit originChanged(this->getOrigin());
    }

    QString Location::getProtocol() const
    {
        return this->m_internal.scheme() + ":";
    }

    QString Location::getHost() const
    {
        int port = this->m_internal.port();

        // If the port is not specified just return the host name
        if(port < 0)
        {
            return this->getHostname();
        }

        return this->m_internal.host() + ":" + QString::number(this->m_internal.port());
    }

    QString Location::getHostname() const
    {
        return this->m_internal.host();
    }

    int Location::getPort() const
    {
        return this->m_internal.port();
    }

    QString Location::getPathname() const
    {
        return this->m_internal.path();
    }

    void Location::setPathname(QString pathname)
    {
        this->m_internal.setPath(pathname);

        emit ancestorOriginsChanged(this->getAncestorOrigins());
        emit hrefChanged(this->getHref());
        emit pathnameChanged(this->getPathname());
        emit searchChanged(this->getSearch());
        emit hashChanged(this->getHash());
    }

    QString Location::getSearch() const
    {
        return this->m_internal.query();
    }

    QString Location::getHash() const
    {
        return this->m_internal.fragment();
    }

    QString Location::getOrigin() const
    {
        return this->m_internal.scheme() + "://" + this->m_internal.authority();
    }

    void Location::reload()
    {
        this->reload(false);
    }

    void Location::reload(bool hardReload)
    {
        emit requiresReload(this->getHref(), hardReload);
    }

    void Location::reload(std::string href)
    {
        this->reload(false);
    }
}
