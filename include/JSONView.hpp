#pragma once

#include "View.hpp"

class JSONView : public View
{
    Q_OBJECT

  public:
    explicit JSONView();

    static QByteArray JSON_VIEW_DOCUMENT;

    /**
     * Returns the name that is displayed in the view selector.
     **/
    QString getDisplayName() const override;

    /**
     * Returns the icon that is displayed in the view selector.
     **/
    QUrl getIconSource() const override;

    /**
     * Generates a QML source string based on the provided QByteArray data.
     **/
    const QByteArray &process(const QByteArray &data) override;
};
