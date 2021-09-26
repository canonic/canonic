#include "../include/HTMLView.hpp"

#include <QFile>

QByteArray HTMLView::HTML_VIEW_DOCUMENT = 0;

HTMLView::HTMLView(): View()
{
    if(HTMLView::HTML_VIEW_DOCUMENT.length() == 0)
    {
        QFile file("://qml/HTMLDocumentView.qml");
        if (file.open(QIODevice::ReadOnly))
        {
            HTMLView::HTML_VIEW_DOCUMENT = file.readAll();
            file.close();
        }
    }
}

QString HTMLView::getDisplayName() const
{
    return "HTML View";
}

QUrl HTMLView::getIconSource() const
{
    return QUrl{""};
}

const QByteArray &HTMLView::process(const QByteArray &data)
{
    return HTMLView::HTML_VIEW_DOCUMENT;
}
