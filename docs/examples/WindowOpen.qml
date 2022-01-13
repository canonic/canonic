// WindowOpen.qml
import QtQuick
import QtQuick.Controls as QtControls

QtControls.Button {
    text: 'Click Me'
    anchors.centerIn: parent

    // Open the Canonic discord link 
    onClicked: window.open("https://discord.gg/YcsEwdaNbR", "", "external=yes")
}