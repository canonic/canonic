import QtQuick 2.15
import QtGraphicalEffects 1.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym
import com.mycompany.qmlcomponents 1.0

Metonym.ThemedItem {
    id: root
    theme: Metonym.CanonicDarkTheme {
        id: theme
    }

    //color: 'transparent'
    anchors.fill: parent

    /*
     * Mouse events that are not handled by MainUI should be passed through to the Viewport
     */
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton


        onClicked: {
        }

        onDoubleClicked: {
            //console.log("double clicked", mouse)
            mainWindow.handleMouseDoubleClickEvent(Qt.point(mouse.x, mouse.y),
                                                   mouse.button,
                                                   mouse.buttons,
                                                   mouse.modifiers,
                                                   mouse.source)
        }

        onEntered: {
            //console.log("entered")
        }

        onExited: {
            //console.log("exited")
        }

        onPositionChanged: {
            //console.log("position changed", mouse)
            mainWindow.handlePositionChangedEvent(Qt.point(mouse.x, mouse.y),
                                                  mouse.button,
                                                  mouse.buttons,
                                                  mouse.modifiers,
                                                  mouse.source)
        }

        onPressAndHold: {
            //console.log("Press And Hold", mouse)
        }

        onPressed: {
            //console.log("pressed", mouse)
            mainWindow.handlePressEvent(Qt.point(mouse.x, mouse.y),
                                        mouse.button,
                                        mouse.buttons,
                                        mouse.modifiers,
                                        mouse.source)
        }

        onReleased: {
            //console.log("released", mouse)
            mainWindow.handleReleaseEvent(Qt.point(mouse.x, mouse.y),
                                          mouse.button,
                                          mouse.buttons,
                                          mouse.modifiers,
                                          mouse.source)
        }

        onWheel: {
            //console.log("wheel", wheel)
            mainWindow.handleWheelEvent(Qt.point(wheel.x, wheel.y),
                                        wheel.pixelDelta,
                                        wheel.angleDelta,
                                        wheel.buttons,
                                        wheel.modifiers,
                                        wheel.inverted,
                                        wheel.source)
        }
    }

    MainUI {}

    Component.onCompleted: {
        window.theme = theme
    }
}
