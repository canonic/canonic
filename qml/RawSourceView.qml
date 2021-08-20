import QtQuick 2.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym

Metonym.ThemedItem {
    id: root

    Rectangle {
        anchors.fill: parent
        color: root.theme.col16
    }

    Flickable {
        anchors.fill: parent

        contentWidth: __content.contentWidth
        contentHeight: __content.contentHeight


        TextEdit {
            id: __content
            text: window.document.rawData
            readOnly: true
            selectByMouse: true

            color: root.theme.col0

            font {
                family: root.theme.font1.regular.name
                pointSize: 9
            }
        }

        Metonym.ScrollBar.vertical: Metonym.ScrollBar {}
        Metonym.ScrollBar.horizontal: Metonym.ScrollBar {}
    }
}
