#pragma once

#include <QObject>
#include <QQmlEngine>

namespace WebAPI {
    class Navigator : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString appVersion READ getAppVersion CONSTANT)
        Q_PROPERTY(QString userAgent READ getUserAgent CONSTANT)
        Q_PROPERTY(QString buildID READ getBuildID CONSTANT)
        Q_PROPERTY(QString qtVersionStr READ getQtVersionStr CONSTANT)

      public:
        Navigator(QObject *parent = nullptr);
        QString getAppVersion() const;
        QString getUserAgent() const;
        QString getBuildID() const;
        QString getQtVersionStr() const;
    };
}

