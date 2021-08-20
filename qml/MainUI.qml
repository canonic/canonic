import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15 as QtLayouts
import 'https://www.canonic.com/metonym/release/Metonym/' as Metonym
import com.mycompany.qmlcomponents 1.0

// Unused import statements included so that the modules are packaged for production
import QtQuick.Particles 2.15

Metonym.ThemedItem {
    id: root

    anchors.fill: parent

    property alias topBarHeight: topBar.height

    Metonym.Pane {
        id: topBar
        height: 44
        width: parent.width

        padding: 0
        radius: 0

        z: 2

        Metonym.Popup {
            id: historyPopup

            property string hoverIcon: ''

            y: parent.height

            theme: Metonym.CanonicLightTheme {}

            width: 400
            height: _historyPopupCol.height

            padding: 0

            focus: true
            closePolicy: Metonym.Popup.CloseOnEscape | Metonym.Popup.CloseOnPressOutside | Metonym.Popup.CloseOnReleaseOutsideParent

            QtLayouts.ColumnLayout {
                id: _historyPopupCol
                width: parent.width

                Repeater {
                    model: historyPopup.opened? mainWindow.historyItems.length : 0

                    delegate: Metonym.Button {
                        id: historyButton
                        QtLayouts.Layout.fillWidth: true
                        height: 40
                        label: {
                            const historyItem = mainWindow.historyItems[index]

                            // If the history item has a title then use that
                            const title = historyItem.title
                            if (title.length > 0)
                            {
                                return title
                            }

                            // Otherwise use it's href
                            return historyItem.location.href
                        }

                        icon {
                            source: {
                                // If the history item is the current item then show a circle
                                if (index === mainWindow.historyItemIndex) {
                                    return root.theme.icons.circle
                                }

                                if (historyButton.hovered)
                                {
                                    return historyPopup.hoverIcon
                                }

                                const historyItem = mainWindow.historyItems[index]

                                // If the history item has a favicon source then use that
                                if (historyItem.faviconSource.toString().length > 0)
                                {
                                    return historyItem.faviconSource
                                }

                                // If all else fails then try to get the favicon from google
                                return "https://s2.googleusercontent.com/s2/favicons?domain_url=" + mainWindow.historyItems[index].location.href
                            }

                            color: historyButton.icon.isFont? 'black' : 'transparent'
                        }

                        theme: historyPopup.theme

                        showBackground: true

                        backgroundColor: historyPopup.theme.setColourAlpha(historyPopup.theme.button.backgroundColor, 0)
                        backgroundHoveredColor: historyPopup.theme.button.backgroundColor

                        pointSize: 8
                        focus: mainWindow.historyItemIndex === index

                        horizontalAlignment: Text.AlignRight
                        LayoutMirroring.enabled: true
                        LayoutMirroring.childrenInherit: true

                        onClicked: {
                            mainWindow.historyItemIndex = index
                            historyPopup.close()
                        }
                    }
                }
            }
        }

        Metonym.Button {
            id: backButtonControl

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: 10
                rightMargin: 2
            }

            icon.source: root.theme.icons.arrow_left
            bordered: true
            radius: width / 2

            width: 36
            height: 36

            onClicked: mainWindow.previousPage()
            onPressAndHold: {
                historyPopup.hoverIcon = root.theme.icons.arrow_left
                historyPopup.open()
            }

            enabled: mainWindow.historyItemIndex > 0
        }

        Metonym.Button {
            id: forwardButtonControl

            anchors {
                left: backButtonControl.right
                verticalCenter: parent.verticalCenter
                leftMargin: 2
                rightMargin: 10
            }

            icon.source: root.theme.icons.arrow_right
            showBackground: true

            onClicked: mainWindow.nextPage()
            onPressAndHold: {
                historyPopup.hoverIcon = root.theme.icons.arrow_right
                historyPopup.open()
            }

            enabled: mainWindow.historyItemIndex < mainWindow.historyItems.length - 1
        }

        Metonym.Button {
            id: refreshButtonControl

            anchors {
                left: forwardButtonControl.right
                verticalCenter: parent.verticalCenter
                leftMargin: 10
                rightMargin: 10
            }

            icon.source: root.theme.icons.reload
            showBackground: true

            onClicked: {
                if ((mouse.button === Qt.LeftButton))
                {
                     window.location.reload(mouse.modifiers === Qt.ControlModifier)
                }
            }
        }

        Metonym.Button {
            id: homeButtonControl

            anchors {
                left: refreshButtonControl.right
                verticalCenter: parent.verticalCenter
                leftMargin: 10
                rightMargin: 10
            }

            icon.source: root.theme.icons.home
            showBackground: true

            onClicked: window.location.href = ''
        }

        AddressBar {
            id: addressBar

            theme: root.theme

            anchors {
                left: homeButtonControl.right
                right: themeButton.left
                verticalCenter: parent.verticalCenter
                leftMargin: 10
                rightMargin: 10
            }            
        }

        Metonym.Button {
            id: themeButton

            width: 36
            height: width
            radius: width / 2
            circularButtonIcon: true
            showBackground: true
            borderIcon: activeFocus

            anchors {
                right: userProfileControl.left
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }

            icon.source: {
                const themeSource = mainWindow.themeSource

                if (themeSource.toString() === Metonym.Constants.CANONIC_LIGHT_THEME_SOURCE)
                    return root.theme.icons.day

                return root.theme.icons.night
            }

            onClicked: {
                const themeSource = mainWindow.themeSource
                if (themeSource.toString() === Metonym.Constants.CANONIC_LIGHT_THEME_SOURCE)
                    mainWindow.setThemeSource(Metonym.Constants.CANONIC_DARK_THEME_SOURCE)
                else
                    mainWindow.setThemeSource(Metonym.Constants.CANONIC_LIGHT_THEME_SOURCE)
            }
        }

        Metonym.Button {
            id: userProfileControl

            width: 36
            height: width

            anchors {
                right: settingsButton.left
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }

            icon.source: mainWindow.auth.avatarImageSource.toString()? mainWindow.auth.avatarImageSource : root.theme.icons.user

            radius: userProfileControl.width/2
            showBackground: true
            circularButtonIcon: true
            borderIcon: userProfileControlPopup.opened

            onClicked: userProfileControlPopup.opened? userProfileControlPopup.close() : userProfileControlPopup.open()

            defaultColor: userProfileControlPopup.opened? defaultFocusedColor : root.theme.button.defaultColor
            hoverColor: userProfileControlPopup.opened? hoverFocusedColor : root.theme.button.hoverColor
            mouseDownColor: userProfileControlPopup.opened? mouseDownFocusedColor : root.theme.button.pressedFocusedColor

            Metonym.BusyIndicator {
                id: __busyIndicator
                running: authComponent.isSigningIn

                hoverEnabled: false

                anchors.centerIn: parent
                width: 42
                height: 42
            }

            Metonym.Popup {
                id: userProfileControlPopup

                x: parent.width - width

                theme: Metonym.CanonicLightTheme {}

                width: 220
                height: 320
                focus: true
                closePolicy: Metonym.Popup.CloseOnEscape | Metonym.Popup.CloseOnPressOutsideParent

                AuthComponent {
                    id: authComponent
                    theme: userProfileControlPopup.theme
                    //layer.enabled: true
                }
            }
        }

        Metonym.Button {
            id: settingsButton

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }

            icon.source: root.theme.icons.bars
            showBackground: true

            onClicked: settingsPopup.opened? settingsPopup.close() : settingsPopup.open()

            Metonym.Popup {
                id: settingsPopup
                // Turn into a menu

                theme: Metonym.CanonicLightTheme{}

                x: parent.width - width

                width: 600
                height: 320
                //focus: true
                closePolicy: Metonym.Popup.CloseOnEscape | Metonym.Popup.CloseOnPressOutsideParent


                Metonym.ComboBox {
                    displayText: 'Build: ' + currentText

                    textRole: "text"
                    valueRole: "value"

                    model: [
                        { value: 'release', text: 'Release' },
                        { value: 'debug', text: 'Debug' }
                    ]

                    onCurrentValueChanged: {
                        mainWindow.build = currentValue
                    }
                }
            }
        }

        Metonym.Pane {
            height: 1

            padding: 0
            radius: 0

            backgroundColor: root.theme instanceof Metonym.CanonicDarkTheme ? root.theme.col15: root.theme.col15

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }
    }

    Metonym.Pane {
        anchors.fill: parent
        layer.enabled: true

        padding: 0
        radius: 0

        Item {
            anchors {
                topMargin: topBar.height
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            Metonym.BusyIndicator {
                running: true
                anchors.centerIn: parent
            }
        }

        opacity: (window.document.readyState === "loading") ? 1 : 0
        visible: opacity > 0

        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutSine
            }
        }
    }
}
