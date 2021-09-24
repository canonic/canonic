/*
#include <QSurfaceFormat>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QWidget>
#include <QVBoxLayout>
*/

#include <QtGlobal>

// Use to test QQuick mode
//#ifdef Q_OS_WASM
//    #undef Q_OS_WASM
//#endif
//#define Q_OS_WASM

/*
#ifdef Q_OS_WASM
    #include <QApplication>
    #include "include/QQuickMainWindow.hpp"
#else
    #include <QApplication>
    #include "include/QWidgetMainWindow.hpp"
#endif
*/

#include <QGuiApplication>
#include <QtQuick3D/qquick3d.h>
#include <QQuickStyle>
#include <QQuickWindow>

#include "include/MainWindow.hpp"
#include "include/Window.hpp"

// Include charts so that it is installed for webassembly
// #include <QtCharts>


int main(int argc, char *argv[])
{
    qSetMessagePattern("%{file}:%{line} - %{message}");

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("://assets/icons/icon.svg"));

    QCoreApplication::setOrganizationName("Canonic");
    QCoreApplication::setApplicationName("Canonic");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    // Set the scaling factor to
    bool ok{false};
    qgetenv("QT_SCALE_FACTOR").toFloat(&ok);
    if(!ok){
        qputenv("QT_SCALE_FACTOR", "1.0");
    }

    QQuickStyle::setStyle("Material");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Dense");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_ACCENT", "#1CFE98");

    // only functional when Qt Quick is also using OpenGL
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);

    MainWindow mainWin;
    /*
    // Set the initial url if provided
    QString initialUrl = QString(qgetenv("CANONIC_INITIAL_URL"));
    if(initialUrl.length())
    {
        mainWin.m_window->getLocation()->setHref(initialUrl);
    }
    else {
        mainWin.m_window->getLocation()->setHref("");
    }*/

    return app.exec();
}
