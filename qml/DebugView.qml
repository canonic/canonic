import QtQuick 2.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym

Item {
    Loader {
        anchors.fill: parent
        active: Metonym.Styles.themesLoaded

        sourceComponent: Component {
            Metonym.ThemedItem {
                id: root

                Rectangle {
                    anchors.fill: parent
                    color: root.theme.col16

                    Grid {
                        width: parent.width

                        columns: 2
                        spacing: 10

                        Metonym.Label {
                            text: 'window.document.rawData.byteLength:'
                        }

                        Metonym.Label {
                            text: JSON.stringify(window.document.rawData.byteLength)
                        }

                        Metonym.Label {
                            text: 'window.document.readyState:'
                        }

                        Metonym.Label {
                            text: JSON.stringify(window.document.readyState)
                        }

                        Metonym.Label {
                            text: 'window.document.title:'
                        }

                        Metonym.Label {
                            text: JSON.stringify(window.document.title)
                        }

                        Metonym.Label {
                            text: 'window.document.URL:'
                        }

                        Metonym.Label {
                            text: JSON.stringify(window.document.URL)
                        }
                    }
                }
            }
        }
    }
}
