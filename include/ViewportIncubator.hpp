#pragma once

#include <QObject>
#include <QQmlIncubator>


class ViewportIncubator: public QObject, public QQmlIncubator
{
    Q_OBJECT

  signals:
    void statusChangedSignal(QQmlIncubator::Status status);

  protected:
    void statusChanged(QQmlIncubator::Status status) override;
};
