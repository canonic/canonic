QT += gui
QT += quick
QT += quick3d
QT += quickcontrols2
QT += websockets


CONFIG += c++11
CONFIG += debug_and_release

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Add Git info variables
GIT_HASH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)\\\""
GIT_BRANCH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --abbrev-ref HEAD)\\\""
DEFINES += GIT_HASH=$$GIT_HASH
DEFINES += GIT_BRANCH=$$GIT_BRANCH

DEFINES += "CANONIC_HOME_PAGE=\"\\\"https://www.canonic.com/\\\"\""

SOURCES += \
        lib/Auth.cpp \
        lib/CanonicRenderer.cpp \
        lib/ContentViewport.cpp \
        lib/CxxSyntaxHighlighter.cpp \
        lib/DebugView.cpp \
        lib/Document.cpp \
        lib/HTMLView.cpp \
        lib/HistoryItem.cpp \
        lib/HostEventPropagator.cpp \
        lib/HostViewport.cpp \
        lib/JSONView.cpp \
        lib/Location.cpp \
        lib/MainWindow.cpp \
        lib/Navigator.cpp \
        lib/NetworkAccessManager.cpp \
        lib/NetworkAccessManagerFactory.cpp \
        lib/ProxyFactory.cpp \
        lib/QMLView.cpp \
        lib/RawSourceView.cpp \
        lib/RenderControl.cpp \
        lib/URISyntaxHighlighter.cpp \
        lib/Viewport.cpp \
        lib/ViewportIncubator.cpp \
        lib/Window.cpp \
        main.cpp

RESOURCES += qml.qrc
#    modules/Metonym/metonym.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
# QML_IMPORT_PATH = modules/

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/Auth.hpp \
    include/CanonicRenderer.hpp \
    include/ContentViewport.fwd.hpp \
    include/ContentViewport.hpp \
    include/CxxSyntaxHighlighter.hpp \
    include/DebugView.hpp \
    include/Document.hpp \
    include/HTMLView.hpp \
    include/HistoryItem.hpp \
    include/HostEventPropagator.fwd.hpp \
    include/HostEventPropagator.hpp \
    include/HostViewport.fwd.hpp \
    include/HostViewport.hpp \
    include/JSONView.hpp \
    include/Location.hpp \
    include/MainWindow.fwd.hpp \
    include/MainWindow.hpp \
    include/Navigator.hpp \
    include/NetworkAccessManager.hpp \
    include/NetworkAccessManagerFactory.hpp \
    include/ProxyFactory.hpp \
    include/QMLView.hpp \
    include/RawSourceView.hpp \
    include/RenderControl.fwd.hpp \
    include/RenderControl.hpp \
    include/URISyntaxHighlighter.hpp \
    include/View.hpp \
    include/Viewport.fwd.hpp \
    include/Viewport.hpp \
    include/ViewportIncubator.fwd.hpp \
    include/ViewportIncubator.hpp \
    include/Window.fwd.hpp \
    include/Window.hpp

DISTFILES += \
    .gitignore \
    README.md \
    build.py \
    build/README.md \
    index.html \
    qml/Meta/qmldir


CONFIG(wasm) {
    QMAKE_LFLAGS += "-s INITIAL_MEMORY=33554432 "

} else {
    QT += quickwidgets
    QT += svg
    QML_IMPORT_PATH = https://www.canonic.com/metonym/release/Metonym/
    QT_PLUGIN += qsvg
}

CONFIG(debug, debug|release) {
    DESTDIR = debug

    #CONFIG(wasm) {
        # wasm source map help http://qtandeverything.blogspot.com/2020/05/debugging-qt-for-webassembly.html
        #QMAKE_WASM_SOURCE_MAP_BASE = https:www.canonic.com/

        #QMAKE_CXXFLAGS += "--source-map-base https://www.canonic.com/"
        #QMAKE_LFLAGS += "--source-map-base https://www.canonic.com/"
    #}
    #QMAKE_CXXFLAGS += "-g4"
    #QMAKE_LFLAGS += "-g4"

    DEFINES += "CANONIC_BUILD=\"\\\"debug\\\"\""
}
CONFIG(release, debug|release) {
    DESTDIR = release

    DEFINES += "CANONIC_BUILD=\"\\\"release\\\"\""
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
