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
            text: "Canonic does not support HTML (yet)"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Metonym.Button {
            label: 'Click to open with default browser'
            bordered: true

            onClicked: {
                const windowFeatures = "external=yes";
                window.open(window.location.href, "windowName", windowFeatures)
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
