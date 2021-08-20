#pragma once

#include <QObject>
#include <QQmlEngine>
#include "Location.hpp"

class HistoryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WebAPI::Location *location READ getLocation CONSTANT)
    Q_PROPERTY(QUrl faviconSource READ getFaviconSource NOTIFY faviconSourceChanged)
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)

  signals:
    void faviconSourceChanged(QUrl faviconSource);
    void titleChanged(QString title);

  public:
    HistoryItem(const WebAPI::Location &location, QObject *parent = nullptr);
    HistoryItem(const QString &href, QObject *parent = nullptr);

    WebAPI::Location *getLocation() const;

    QUrl getFaviconSource() const;
    void setFaviconSource(QUrl faviconSource);

    QString getTitle() const;
    void setTitle(QString title);

  private:
    WebAPI::Location *m_location;
    QUrl m_faviconSource;
    QString m_title;
};
