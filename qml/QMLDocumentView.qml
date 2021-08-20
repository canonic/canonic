import QtQuick 2.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym

Metonym.ThemedItem {
    id: root

    FocusScope {
        id: focusSink
        Item {
            focus: true
        }
    }

    MouseArea {
        anchors.fill: root
        onClicked: {
            loader.forceActiveFocus()
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
        focus: true // Must be set for any children to get active focus

        readonly property url loaderSource: {
            const href = window.location.href

            if (href === '')
            {
                return mainWindow.homePage
            }

            return href
        }

        source: loaderSource

        onStatusChanged: {
            const status = loader.status

            if (loader.status == Loader.Ready)
            {
                const loadedItem = loader.item

                if (loadedItem.hasOwnProperty('meta'))
                {
                    const meta = loadedItem.meta
                    if (meta.hasOwnProperty('title'))
                    {
                        const title = meta.title
                        if (typeof title === 'string')
                        {
                            window.document.title = meta.title
                        }

                        const faviconSource = meta.faviconSource
                        if (typeof faviconSource === 'string')
                        {
                            window.document.faviconSource = meta.faviconSource
                        }
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        layer.enabled: true

        Metonym.BusyIndicator {
            running: true
            anchors.centerIn: parent
        }

        opacity: (loader.status === Loader.Loading) ? 1 : 0
        visible: opacity > 0

        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutSine
            }
        }
    }


    // I think Dynamic qml object creation might be causing a hang, but it might be fine up to you if
    // you prefer this method
    /*
    Component.onCompleted: {
        console.log("QMLDocumentView completed")
        const newObject = Qt.createQmlObject(window.document.rawData, contents, window.location.href);

        console.log("newObject", newObject)

        // Fill by default
        if(newObject)
        {
            console.log("set anchors fill")
            newObject.anchors.fill = Qt.binding(function() { return contents })
        }

        if (newObject.hasOwnProperty('meta'))
        {
            const meta = newObject.meta
            if (meta.hasOwnProperty('title'))
            {
                const title = meta.title
                if (typeof title === 'string')
                {
                    window.document.title = meta.title
                }

                const faviconSource = meta.faviconSource
                if (typeof faviconSource === 'string')
                {
                    window.document.faviconSource = meta.faviconSource
                }
            }
        }

        console.log("QMLDocumentView completed completed")
    }
    */
}
