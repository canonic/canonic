#include "../include/HistoryItem.hpp"

HistoryItem::HistoryItem(const WebAPI::Location &location, QObject *parent): QObject(parent)
{
    HistoryItem(location.getHref(), parent);
}

HistoryItem::HistoryItem(const QString &href, QObject *parent): QObject(parent)
{
    this->m_location = new WebAPI::Location(href);
}

WebAPI::Location *HistoryItem::getLocation() const {
    return m_location;
}

QUrl HistoryItem::getFaviconSource() const
{
    return this->m_faviconSource;
}

void HistoryItem::setFaviconSource(QUrl faviconSource)
{
    this->m_faviconSource = faviconSource;
    emit this->faviconSourceChanged(faviconSource);
}

QString HistoryItem::getTitle() const
{
    return this->m_title;
}

void HistoryItem::setTitle(QString title)
{
    this->m_title = title;
    emit this->titleChanged(title);
}
