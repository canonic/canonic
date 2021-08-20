import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15 as QtControls
import QtQuick.Window 2.15 as QtWindow
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym
import com.mycompany.qmlcomponents 1.0

Item {
    id: root

    readonly property var theme: mainWindow.themeComponent.createObject(this)

    anchors.fill: parent

    z: 1
    layer.enabled: true

    Rectangle {
        color: root.theme.col16
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: layoutLoader
        onClicked: layoutLoader.forceActiveFocus()
    }

    Loader {
        id: layoutLoader

        anchors {
            fill: parent
            topMargin: mainUI.topBarHeight
        }

        z: 2

        asynchronous: true
        source: {
            if(mainWindow.activeViewIndex < 0 || mainWindow.activeViewIndex >= mainWindow.views.length)
            {
                return ''
            }

            return mainWindow.views[mainWindow.activeViewIndex].qmlSource
        }

        // Force active focus on load
        onLoaded: layoutLoader.forceActiveFocus()

        active: mainWindow.views.length > 0
    }

    MainUI {
        id: mainUI
        z: 3
        theme: root.theme
        layer.enabled: true
    }

    Component.onCompleted: {
        window.theme = theme
        mainWindow.mainUILoaded()
    }
}
