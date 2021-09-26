#include "../include/JSONView.hpp"

#include <QFile>

QByteArray JSONView::JSON_VIEW_DOCUMENT = 0;

JSONView::JSONView(): View()
{
    if(JSONView::JSON_VIEW_DOCUMENT.length() == 0)
    {
        QFile file("://qml/JSONFileView.qml");
        if (file.open(QIODevice::ReadOnly))
        {
            JSONView::JSON_VIEW_DOCUMENT = file.readAll();
            file.close();
        }
    }
}

QString JSONView::getDisplayName() const
{
    return "JSON View";
}

QUrl JSONView::getIconSource() const
{
    return QUrl{""};
}

const QByteArray &JSONView::process(const QByteArray &data)
{
    return JSONView::JSON_VIEW_DOCUMENT;
}
