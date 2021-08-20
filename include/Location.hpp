#pragma once

#include <QObject>
#include <QQmlEngine>

namespace WebAPI {
    class Location : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString ancestorOrigins READ getAncestorOrigins NOTIFY ancestorOriginsChanged)
        Q_PROPERTY(QString href READ getHref WRITE setHref NOTIFY hrefChanged)
        Q_PROPERTY(QString protocol READ getProtocol NOTIFY protocolChanged)
        Q_PROPERTY(QString host READ getHost NOTIFY hostChanged)
        Q_PROPERTY(QString hostname READ getHostname NOTIFY hostnameChanged)
        Q_PROPERTY(int port READ getPort NOTIFY portChanged)
        Q_PROPERTY(QString pathname READ getPathname WRITE setPathname NOTIFY pathnameChanged)
        Q_PROPERTY(QString search READ getSearch NOTIFY searchChanged)
        Q_PROPERTY(QString hash READ getHash NOTIFY hashChanged)
        Q_PROPERTY(QString origin READ getOrigin NOTIFY originChanged)

      public:
        Location(QObject *parent = nullptr);
        Location(QString location, QObject *parent = nullptr);

        QString getAncestorOrigins() const;
        QString getHref() const;
        void setHref(QString href);
        QString getProtocol() const;
        QString getHost() const;
        QString getHostname() const;
        int getPort() const;
        QString getPathname() const;
        void setPathname(QString pathname);
        QString getSearch() const;
        QString getHash() const;
        QString getOrigin() const;

      public slots:
        void reload();
        void reload(bool hardReload);

      private slots:
        void reload(std::string href);

      signals:
        void ancestorOriginsChanged(QString ancestorOrigins);
        void hrefChanged(QString href);
        void protocolChanged(QString protocol);
        void hostChanged(QString host);
        void hostnameChanged(QString hostname);
        void portChanged(int port);
        void pathnameChanged(QString pathname);
        void searchChanged(QString search);
        void hashChanged(QString hash);
        void originChanged(QString origin);
        void requiresReload(QString href, bool hardReload);

      private:
        QUrl m_internal;
    };
}

