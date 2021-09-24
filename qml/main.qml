import QtQuick 2.15
import QtQuick.Controls 2.15 as QtControls
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym
import com.mycompany.qmlcomponents 1.0

Item {
    id: root

    readonly property var theme: mainWindow.themeComponent.createObject(this)

    HostEventPropagator {
        id: hostEventPropagator
        anchors.fill: parent

        // The HostEventPropagator acts as a focus sink, this also enables the
        // HostEventPropagator to forward KeyEvents.
        onClearHostViewportFocus: hostEventPropagator.forceActiveFocus()
    }

    MainUI {
        theme: root.theme
    }

    Component.onCompleted: {
        window.theme = theme
        //mainWindow.mainUILoaded()
    }

    layer.enabled: true
}
