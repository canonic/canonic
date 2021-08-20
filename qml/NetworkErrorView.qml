import QtQuick 2.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym

Metonym.ThemedItem {
    id: root

    Rectangle {
        anchors.fill: parent
        color: root.theme.col16
    }

    Column {
        anchors.centerIn: parent

        spacing: 5

        Metonym.Label {
            text: "Error Loading Page"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Metonym.Button {
            label: 'Try Again'
            bordered: true
            onClicked: window.location.reload(true)
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
