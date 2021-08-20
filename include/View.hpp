#pragma once

#include <QObject>
#include <QUrl>

class View : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayName READ getDisplayName CONSTANT)
    Q_PROPERTY(QUrl iconSource READ getIconSource CONSTANT)
    Q_PROPERTY(QUrl qmlSource READ getQmlSource CONSTANT)
  public:
    View();
    View(const QString &displayName, const QUrl &iconSource, const QUrl &qmlSource);
    QString getDisplayName() const;
    QUrl getIconSource() const;
    QUrl getQmlSource() const;

  private:
    const QString m_displayName;
    const QUrl m_iconSource;
    const QUrl m_qmlSource;
};
