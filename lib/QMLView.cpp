#include "../include/QMLView.hpp"

QMLView::QMLView(): View()
{
}


QString QMLView::getDisplayName() const
{
    return "QML View";
}


QUrl QMLView::getIconSource() const
{
    return QUrl{""};
}

const QByteArray &QMLView::process(const QByteArray &data)
{
    return data;
}
