#pragma once

#include <QObject>
#include <QQmlComponent>
#include <QUrl>

#include "./MainWindow.fwd.hpp"

class View : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayName READ getDisplayName CONSTANT)
    Q_PROPERTY(QUrl iconSource READ getIconSource CONSTANT)

public:
    /**
     * Returns the name that is displayed in the view selector.
     **/
    virtual QString getDisplayName() const = 0;

    /**
     * Returns the icon that is displayed in the view selector.
     **/
    virtual QUrl getIconSource() const = 0;

    /**
     * Generates a QML source string based on the provided QByteArray data.
     **/
    virtual const QByteArray &process(const QByteArray &data) = 0;
};
