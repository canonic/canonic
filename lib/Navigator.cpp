#include "../include/Navigator.hpp"

#include <QTime>
#include <QDateTime>

namespace WebAPI {
    Navigator::Navigator(QObject *parent) : QObject(parent)
    {
    }

    QString Navigator::getAppVersion() const
    {
        return "0.0.1";
    }

    QString Navigator::getBuildID() const
    {
        return "20181001000000";
    }

    QString Navigator::getUserAgent() const
    {
        return "Canonic";
    }

    QString Navigator::getQtVersionStr() const
    {
        return QT_VERSION_STR;
    }
}
