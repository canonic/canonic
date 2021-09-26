#include "../include/DebugView.hpp"

#include <QFile>

QByteArray DebugView::DEBUG_VIEW_DOCUMENT = 0;

DebugView::DebugView(): View()
{
    if(DebugView::DEBUG_VIEW_DOCUMENT.length() == 0)
    {
        QFile file("://qml/DebugView.qml");
        if (file.open(QIODevice::ReadOnly))
        {
            DebugView::DEBUG_VIEW_DOCUMENT = file.readAll();
            file.close();
        }
    }
}

QString DebugView::getDisplayName() const
{
    return "Debug View";
}

QUrl DebugView::getIconSource() const
{
    return QUrl{""};
}

const QByteArray &DebugView::process(const QByteArray &data)
{
    return DebugView::DEBUG_VIEW_DOCUMENT;
}
