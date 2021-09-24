#pragma once

#include <QObject>

#include "./Document.hpp"
#include "./Location.hpp"
#include "./MainWindow.hpp"
#include "./Navigator.hpp"

namespace WebAPI {
    class Window : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(Location *location READ getLocation WRITE setLocation NOTIFY locationChanged)
        Q_PROPERTY(Navigator *navigator READ getNavigator CONSTANT)
        Q_PROPERTY(int innerScreenX READ getInnerScreenX NOTIFY innerScreenXChanged)
        Q_PROPERTY(int innerScreenY READ getInnerScreenY NOTIFY innerScreenYChanged)
        Q_PROPERTY(int innerWidth READ getInnerWidth NOTIFY innerWidthChanged)
        Q_PROPERTY(int innerHeight READ getInnerHeight NOTIFY innerHeightChanged)
        Q_PROPERTY(int outerWidth READ getOuterWidth NOTIFY outerWidthChanged)
        Q_PROPERTY(int outerHeight READ getOuterHeight NOTIFY outerHeightChanged)
        Q_PROPERTY(Document *document READ getDocument NOTIFY documentChanged)
        Q_PROPERTY(QUrl themeSource READ getThemeSource WRITE setThemeSource NOTIFY themeSourceChanged)
        Q_PROPERTY(QUrl viewSource READ getViewSource NOTIFY viewSourceChanged)
        Q_PROPERTY(QQmlComponent *themeComponent READ getThemeComponent NOTIFY themeComponentChanged)

      public:
        explicit Window(MainWindow *mainWindow, QObject *parent = nullptr);

        Location *getLocation() const;
        void setLocation(Location *location);

        Navigator *getNavigator() const;

        int getInnerScreenX() const;
        int getInnerScreenY() const;
        int getInnerWidth() const;
        int getInnerHeight() const;
        int getOuterWidth() const;
        int getOuterHeight() const;
        void handleWindowResize();
        Document *getDocument() const;

        QUrl getThemeSource() const;
        void setThemeSource(QUrl themeSource);

        QUrl getViewSource() const;

        QQmlComponent *getThemeComponent();


        Q_INVOKABLE
        QString btoa(QString str) const;

        Q_INVOKABLE
        QString atob(QString str) const;

        Q_INVOKABLE
        void open(QString url, QString windowName = "", QString windowFeatures = "");

      signals:
        void locationChanged(WebAPI::Location *location);
        void innerScreenXChanged(int location);
        void innerScreenYChanged(int location);
        void innerWidthChanged(int location);
        void innerHeightChanged(int location);
        void outerWidthChanged(int location);
        void outerHeightChanged(int location);
        void documentChanged(WebAPI::Document *document);
        void themeSourceChanged();
        void viewSourceChanged();
        void themeComponentChanged();

      private slots:
        void handleLocationHrefChange(QString href, bool hardReload);
        void handleFinishedLoadingReply();

      private:
        QNetworkReply *m_networkReply{nullptr};
        Document *m_document;
        Location *m_location;
        Navigator *m_navigator;
        MainWindow *m_mainWindow;
    };
}

