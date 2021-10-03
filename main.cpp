#include <QtGlobal>

#include <QGuiApplication>
#include <QtQuick3D/qquick3d.h>
#include <QQuickStyle>
#include <QQuickWindow>

#include "include/MainWindow.hpp"
#include "include/Window.hpp"

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

    return app.exec();
}
