import QtQuick 2.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym

/**
 * Top Level Item
 **/
Rectangle {
    id: root

    color: 'green'
    readonly property QtObject theme: Metonym.CanonicDarkTheme {}
    property alias source: loader.source

    // Urls may not link to a document that specifies a background colour. In
    // those cases we should display a white background to be consistent with
    // current browsers and default qt qml behavior.
    Rectangle {
        color: loader.status === Loader.Ready? 'white' : root.theme.col16
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: loader
        onClicked: loader.forceActiveFocus()
    }

    Loader {
        id: loader

        y: window.innerScreenY
        x: window.innerScreenX
        width: window.innerWidth
        height: window.innerHeight

        onLoaded: loader.forceActiveFocus()

        source: window.viewSource

        z: 2
        asynchronous: true
    }

    layer.enabled: true

    Component.onCompleted: console.log("TLI component completed")
}
