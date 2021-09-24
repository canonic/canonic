import QtQuick 2.15
import QtQuick.Controls 2.12 as QtControls

Rectangle {
    color: 'blue'

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("Clicked Scroll tester!!!!!!!")
    }

    Flickable {
        anchors.fill: parent

        QtControls.ScrollBar.vertical: QtControls.ScrollBar {}

        contentWidth: width
        contentHeight: mainContent.height

        Column {
            id: mainContent
            spacing: 10
            Repeater {
                model: 50

                delegate: Rectangle {
                    width: 200
                    height: 50
                    color: 'yellow'
                }
            }
        }
    }
}
