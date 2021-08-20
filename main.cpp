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
#include "include/QQuickMainWindow.hpp"
#include "include/Window.hpp"

// Include charts so that it is installed for webassembly
// #include <QtCharts>


int main(int argc, char *argv[])
{
    qSetMessagePattern("%{file}:%{line} - %{message}");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("://assets/icons/icon.svg"));

    QCoreApplication::setOrganizationName("Canonic");
    QCoreApplication::setApplicationName("Canonic");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    // Set the scaling factor to
    bool ok{false};
    float currentScalingFactor = qgetenv("QT_SCALE_FACTOR").toFloat(&ok);
    if(!ok){
        qputenv("QT_SCALE_FACTOR", "1.0");
    }

    /*
#ifdef Q_OS_WASM
    QQuickMainWindow mainWin;
#else
    QWidgetMainWindow mainWin;
#endif
    */

    QQuickMainWindow mainWin;

    mainWin.resize(1600, 1000);
    mainWin.show();

    // Set the initial url if provided
    QString initialUrl = QString(qgetenv("CANONIC_INITIAL_URL"));
    if(initialUrl.length())
    {
        mainWin.m_window->getLocation()->setHref(initialUrl);
    }
    else {
        mainWin.m_window->getLocation()->setHref("");
    }

    return app.exec();
}

/*
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    // Set multisampling
    //QSurfaceFormat format;
    //format.setSamples(16);
    //QSurfaceFormat::setDefaultFormat(format);

    // Ignore SSL Errors [Comment out before production]

    //QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    //sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    //QSslConfiguration::setDefaultConfiguration(sslConf);

    QScopedPointer<MetaAPI> metaAPI(new MetaAPI);


    QQmlApplicationEngine qmlEngine;

    // Set the qml engine's networkAccessManagerFactory
    NetworkAccessManagerFactory *networkAccessManagerFactory = new NetworkAccessManagerFactory();
    qmlEngine.setNetworkAccessManagerFactory(networkAccessManagerFactory);

    // Get the qml engine's rootContext
    QQmlContext *rootContext{qmlEngine.rootContext()};

    Constants::registerSingleton(&qmlEngine);
    Utils::registerSingleton(&qmlEngine);
    Theme::registerSingleton(&qmlEngine);

    Location *locationInstance(new Location());

    window * windowInstance{new window(locationInstance, &qmlEngine)};
    rootContext->setContextProperty("window", windowInstance);

    qmlRegisterSingletonInstance("Qt.Meta.API", 1, 0, "MetaAPI", metaAPI.get());
    qmlRegisterType< CxxSyntaxHighlighter >( "Meta", 1, 0, "CxxSyntaxHighlighter" );

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));

    qmlEngine.load(url);

    //SslEchoClient client(QUrl(QStringLiteral("wss://localhost:9767")));
    //Q_UNUSED(client);

    return app.exec();
}
*/
