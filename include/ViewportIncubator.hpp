#pragma once

#include <QObject>
#include <QQmlIncubator>

#include "MainWindow.fwd.hpp"


class ViewportIncubator: public QObject, public QQmlIncubator
{
    Q_OBJECT

  signals:
    void statusChangedSignal(QQmlIncubator::Status status);

  public:
    explicit ViewportIncubator(MainWindow *mainWindow);

  protected:
    void statusChanged(QQmlIncubator::Status status) override;
    void setInitialState(QObject *object) override;

  private:
    MainWindow *m_mainWindow;
};
