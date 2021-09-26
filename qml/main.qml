import QtQuick 2.15
import QtQuick.Controls 2.15 as QtControls
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym
import com.mycompany.qmlcomponents 1.0

Item {
    id: root

    HostEventPropagator {
        id: hostEventPropagator
        anchors.fill: parent

        // The HostEventPropagator acts as a focus sink, this also enables the
        // HostEventPropagator to forward KeyEvents.
        onClearHostViewportFocus: hostEventPropagator.forceActiveFocus()
    }

    Loader {
        anchors.fill: parent
        sourceComponent: Component {
            MainUI {
                theme: Metonym.Styles.theme
            }
        }
        active: Metonym.Styles.themesLoaded
    }

    layer.enabled: true
}
