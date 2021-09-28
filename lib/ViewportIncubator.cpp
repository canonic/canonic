#include "../include/ViewportIncubator.hpp"
#include "../include/MainWindow.hpp"

#include <QDebug>
#include <QQuickItem>
#include <QQuickWindow>

ViewportIncubator::ViewportIncubator(MainWindow *mainWindow): m_mainWindow{mainWindow}
{
}

void ViewportIncubator::statusChanged(QQmlIncubator::Status status)
{
    emit this->statusChangedSignal(status);
}

void ViewportIncubator::setInitialState(QObject *object)
{
    // QQuickWindows need to be displayed as sub windows
    QQuickWindow *castWindow = qobject_cast<QQuickWindow *>(object);
    if (castWindow) {
        castWindow->setFlags(Qt::SubWindow);
        castWindow->setParent(this->m_mainWindow);
        castWindow->setTransientParent(this->m_mainWindow);
    }
    return;
}
