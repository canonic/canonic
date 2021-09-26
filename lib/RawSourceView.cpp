#include "../include/RawSourceView.hpp"

#include <QFile>

QByteArray RawSourceView::RAW_SOURCE_VIEW_DOCUMENT = 0;

RawSourceView::RawSourceView(): View()
{
    if(RawSourceView::RAW_SOURCE_VIEW_DOCUMENT.length() == 0)
    {
        QFile file("://qml/RawSourceView.qml");
        if (file.open(QIODevice::ReadOnly))
        {
            RawSourceView::RAW_SOURCE_VIEW_DOCUMENT = file.readAll();
            file.close();
        }
    }
}

QString RawSourceView::getDisplayName() const
{
    return "Raw Source View";
}

QUrl RawSourceView::getIconSource() const
{
    return QUrl{""};
}

const QByteArray &RawSourceView::process(const QByteArray &data)
{
    return RawSourceView::RAW_SOURCE_VIEW_DOCUMENT;
}
