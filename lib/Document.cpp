#include "../include/Document.hpp"
#include "../include/MainWindow.hpp"

namespace WebAPI {
    Document::Document(MainWindow *mainWindow): m_mainWindow{mainWindow}
    {
    }

    QByteArray Document::getRawData() const
    {
        return this->m_rawData;
    }

    void Document::setRawData(const QByteArray &rawData)
    {
        this->m_rawData.clear();
        this->m_rawData.append(rawData);
        emit rawDataChanged();
    }

    QJsonObject Document::getObjectType() const
    {
        return this->m_objectType;
    }

    void Document::setObjectType(QJsonObject objectType)
    {
        this->m_objectType = objectType;
        emit objectTypeChanged(this->m_objectType);
    }

    QJsonObject Document::getObjectValue() const
    {
        return this->m_objectValue;
    }

    void Document::setObjectValue(QJsonObject m_objectValue)
    {
        this->m_objectValue = m_objectValue;
        emit objectValueChanged(this->m_objectValue);
    }

    QString Document::getReadyState() const
    {
        return this->m_readyState;
    }

    void Document::setReadyState(QString readyState)
    {
        this->m_readyState = readyState;
        emit this->readyStateChanged(readyState);
    }

    QString Document::getTitle() const
    {
        return this->m_title;
    }

    void Document::setTitle(QString title)
    {
        this->m_title = title;

        // Set the title for any history items with this location
        const QVector<HistoryItem *> history = this->m_mainWindow->getHistory();
        for (HistoryItem *historyItem : history)
        {
            if (historyItem->getLocation()->getHref() == this->getURL())
            {
                historyItem->setTitle(title);
            }
        }

        emit this->titleChanged(title);
    }

    QString Document::getURL() const
    {
        return this->m_URL;
    }

    void Document::setURL(QString URL)
    {
        this->m_URL = URL;
        emit this->URLChanged(URL);
    }

    QUrl Document::getFaviconSource() const
    {
        return this->m_faviconSource;
    }

    void Document::setFaviconSource(QUrl faviconSource)
    {
        this->m_faviconSource = faviconSource;

        // Set the favicon source for any history items with this location
        const QVector<HistoryItem *> history = this->m_mainWindow->getHistory();
        for (HistoryItem *historyItem : history)
        {
            if (historyItem->getLocation()->getHref() == this->getURL())
            {
                historyItem->setFaviconSource(faviconSource);
            }
        }

        emit this->faviconSourceChanged(m_faviconSource);
    }
}
