#include "../include/View.hpp"

View::View(): m_displayName{""},
                             m_iconSource{""},
                             m_qmlSource{""}
{
}

View::View(const QString &displayName,
    const QUrl &iconSource,
    const QUrl &qmlSource): m_displayName{displayName},
                             m_iconSource{iconSource},
                             m_qmlSource{qmlSource}
{
}

QString View::getDisplayName() const
{
    return this->m_displayName;
}

QUrl View::getIconSource() const
{
    return this->m_iconSource;
}

QUrl View::getQmlSource() const
{
    return this->m_qmlSource;
}
