// SetWindowLocationHref.qml
import QtQuick
import QtQuick.Controls as QtControls

QtControls.Button {
    text: 'Click Me'
    anchors.centerIn: parent

    // Change url on click
    onClicked: window.location.href = "https://discord.gg/YcsEwdaNbR"
}