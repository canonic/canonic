#pragma once

#include <QObject>
#include <QJsonObject>

#include "../include/MainWindow.fwd.hpp"

namespace WebAPI {
    class Document : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QByteArray rawData READ getRawData NOTIFY rawDataChanged)
        Q_PROPERTY(QJsonObject objectType READ getObjectType NOTIFY objectTypeChanged)
        Q_PROPERTY(QJsonObject objectValue READ getObjectValue NOTIFY objectValueChanged)
        Q_PROPERTY(QString readyState READ getReadyState NOTIFY readyStateChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(QString URL READ getURL NOTIFY URLChanged)
        Q_PROPERTY(QUrl faviconSource READ getFaviconSource WRITE setFaviconSource NOTIFY faviconSourceChanged)

      public:
        Document(MainWindow *mainWindow);

        QByteArray getRawData() const;
        void setRawData(const QByteArray &rawData);

        QJsonObject getObjectType() const;
        void setObjectType(QJsonObject objectType);

        QJsonObject getObjectValue() const;
        void setObjectValue(QJsonObject objectValue);

        QString getReadyState() const;
        void setReadyState(QString readyState);

        QString getTitle() const;
        void setTitle(QString title);

        QString getURL() const;
        void setURL(QString URL);

        QUrl getFaviconSource() const;
        void setFaviconSource(QUrl faviconSource);

       signals:
         void rawDataChanged();
         void objectTypeChanged(QJsonObject objectType);
         void objectValueChanged(QJsonObject objectValue);
         void readyStateChanged(QString readyState);
         void titleChanged(QString title);
         void URLChanged(QString URL);
         void faviconSourceChanged(QUrl faviconSource);

       private:
         QByteArray m_rawData;
         QJsonObject m_objectType;
         QJsonObject m_objectValue;
         QString m_readyState;
         QString m_title;
         QString m_URL;
         QUrl m_faviconSource;
         MainWindow *m_mainWindow;

    };
}
