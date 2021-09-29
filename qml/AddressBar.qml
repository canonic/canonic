import QtQuick 2.15
import QtQuick.Controls 2.12
import Qt5Compat.GraphicalEffects as QtGraphicalEffects
import Meta 1.0
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym
import com.mycompany.qmlcomponents 1.0

Metonym.ThemedItem {
    id: root
    implicitHeight: 28
    implicitWidth: 300

    readonly property color backgroundColor: root.theme instanceof Metonym.CanonicDarkTheme ? root.theme.col17: root.theme.getInterpolatedColour(17.5)
    readonly property color placeholderColor: root.theme instanceof Metonym.CanonicDarkTheme ? root.theme.col8: root.theme.col10

    Component {
        id: _addressBarInternal

        FocusScope {
            id: _internalRoot
            implicitHeight: parent.height
            implicitWidth: parent.width

            property alias url: addressBarTextField.text

            property bool containsFocus: _internalRoot.activeFocus

            Timer {
                id: selectAllTimer
                interval: 100
                running: false
                repeat: false
                onTriggered: addressBarTextField.selectAll()
            }

            QtGraphicalEffects.Glow {
                id: __borderGlow

                anchors.fill: __addressBarRect

                radius: 4
                color: _internalRoot.containsFocus ? root.theme.brand : root.theme.col0

                opacity: __addressBarHoverHandler.hovered? 1 : 0

                Behavior on opacity {
                    NumberAnimation {
                        duration: 100
                        easing.type: Easing.InOutSine
                    }
                }

                spread: 0.3
                source: __addressBarRect
                visible: true
            }

            Rectangle {
                id: __addressBarRect

                anchors {
                    fill: parent
                    margins: _internalRoot.containsFocus? -5: 0

                    Behavior on margins {
                        NumberAnimation {
                            duration: 100
                            easing.type: Easing.InOutSine
                        }
                    }
                }
                color: root.backgroundColor

                radius: _internalRoot.containsFocus? 1: 2

                Behavior on radius {
                    NumberAnimation {
                        duration: 100
                        easing.type: Easing.InOutSine
                    }
                }

                border {
                    color: {
                        if(_internalRoot.containsFocus)
                        {
                            if(__addressBarHoverHandler.hovered)
                            {
                                return root.theme.brandHighlight
                            }

                            return root.theme.brand
                        }

                        if(__addressBarHoverHandler.hovered)
                        {
                            return root.theme.border.borderColorHovered
                        }

                        return root.theme.border.borderColor
                    }

                    Behavior on color {
                        ColorAnimation {
                            duration: 100
                            easing.type: Easing.InOutSine
                        }
                    }

                    width: 1
                }
            }

            Item {
                anchors.fill: parent
                clip: true

                Metonym.Icon {
                    id: addressBarIcon

                    anchors {
                        verticalCenter: parent.verticalCenter
                    }

                    color: placeholder.color

                    x: 6
                    width: 24
                    height: 24

                    source: root.theme.icons.search
                    horizontalAlignment : Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                }

                TextEdit {
                    id: addressBarTextField

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: addressBarIcon.right
                        leftMargin: 4
                        right: __addressBarRightControls.left
                    }

                    clip: true


                    verticalAlignment: TextEdit.AlignVCenter

                    font.family: Metonym.Styles.monterratRegular.name
                    font.pointSize: 10
                    font.kerning: false  // disable kerning so fontMetrics.advanceWidth lines up correctly


                    selectByMouse: true

                    color: root.theme.col0
                    selectionColor: Metonym.Styles.brandColor
                    selectedTextColor: root.theme.col19

                    text: window.location.href


                    onActiveFocusChanged: {
                        var containsFocus = addressBarTextField.activeFocus
                        syntaxHighlighter.isFocused = containsFocus

                        if(containsFocus)
                        {
                            // Timer is required since TextEdit updates the selection internally after focus
                            selectAllTimer.start()
                        }
                        else{
                            selectAllTimer.stop()
                        }

                        rehighlightWorkaround()
                    }

                    function rehighlightWorkaround() {
                        // rehighlight does not work...
                        //syntaxHighlighter.rehighlight()

                        // so instead append an empty string
                        addressBarTextField.append("")
                    }


                    onTextChanged: {
                        // Sanitise multi line paste input
                        var newCursorPos = addressBarTextField.cursorPosition
                        var newTxt = addressBarTextField.text

                        var modifiedCursorPos = newCursorPos
                        var modifiedTxt = "";
                        var currentLength = 0
                        for( var i = 0; i < newTxt.length; i++ )
                        {
                            if( !(newTxt[i] === '\n' || newTxt[i] === '\r') )
                            {
                                modifiedTxt += newTxt[i];
                                currentLength ++
                            }
                            else
                            {
                                if(i < newCursorPos)
                                {
                                    modifiedCursorPos--;
                                }
                            }
                        }

                        addressBarTextField.text = modifiedTxt
                        addressBarTextField.cursorPosition = modifiedCursorPos
                    }

                    Keys.onReturnPressed: {
                        window.location.href = addressBarTextField.text
                        event.accepted = true;
                    }

                    URISyntaxHighlighter {
                        id: syntaxHighlighter
                        textDocument: addressBarTextField.textDocument

                        defaultColor: root.theme instanceof Metonym.CanonicDarkTheme ? root.theme.col4: root.theme.col8
                        highlightForgroundColor: root.theme.col0

                        onDefaultColorChanged: addressBarTextField.rehighlightWorkaround()
                        onHighlightForgroundColorChanged: addressBarTextField.rehighlightWorkaround()
                    }
                }

                Row {
                    id: __addressBarRightControls

                    anchors {
                        rightMargin: 1
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }

                    clip: true

                    Metonym.Button {
                        showBackground: true
                        radius: 0

                        anchors.verticalCenter: parent.verticalCenter
                        height: parent.height - 2

                        horizontalPadding: 0
                        icon.source: root.theme.icons.view

                        backgroundColor: window.theme === Metonym.Constants.CANONIC_THEME_DARK? root.theme.colourMain(0.4): root.theme.colourMain(0.9)
                        backgroundHoveredColor: root.theme.col13
                        backgroundPressedColor: root.theme.col15

                        onClicked: viewMenu.visible? viewMenu.close() : viewMenu.open()

                        enabled: mainWindow.views.length > 0

                        Metonym.Popup {
                            id: viewMenu

                            theme: Metonym.Styles.lightThemeLoader.item

                            y: parent.height + 10
                            x: parent.width - width

                            padding: 10
                            width: _viewMenuContent.width + padding * 2
                            height: _viewMenuContent.height + padding * 2

                            modal: true
                            closePolicy: Metonym.Popup.CloseOnEscape | Metonym.Popup.CloseOnPressOutsideParent

                            radius: 5

                            Column {
                                id: _viewMenuContent

                                width: 300

                                spacing: 5

                                Repeater {
                                    id: _viewRepeater
                                    width: parent.width
                                    model: mainWindow.views.length
                                    delegate: Metonym.Button {
                                        property var __app: mainWindow.views[model.index]
                                        label: __app.displayName? __app.displayName : __app.source

                                        theme: Metonym.Styles.lightThemeLoader.item

                                        width: parent.width
                                        pointSize: 9
                                        bold: false
                                        horizontalAlignment: Text.AlignRight
                                        icon.source: mainWindow.activeViewIndex === model.index ? theme.icons.circle : theme.icons.empty
                                        showBackground: true
                                        backgroundColor: theme.popup.backgroundColor

                                        LayoutMirroring.enabled: true
                                        LayoutMirroring.childrenInherit: true

                                        onClicked: mainWindow.activeViewIndex = model.index
                                    }
                                }
                            }
                        }
                    }
                }

                Text {
                    id: placeholder
                    anchors.fill: addressBarTextField
                    font: addressBarTextField.font
                    text: "Search"
                    color: root.placeholderColor
                    verticalAlignment: addressBarTextField.verticalAlignment
                    visible: addressBarTextField.text.length <= 0
                }
            }

            DropArea {
                id: dropArea
                anchors.fill: parent

                onDropped: {
                    if(drop.urls.length > 0)
                    {
                        window.location.href = drop.urls[0]
                    }
                }
            }

            Connections {
                target: window.location

                function onHrefChanged() {
                    _internalRoot.url = window.location.href
                }
            }
        }
    }

    QtObject {
        id: _hiddenProperties
        property Item _addressBarInternalInstance: Loader {
            anchors.fill: parent
            sourceComponent: _addressBarInternal
        }
    }

    Item {
        id: _addressBarContainer
        anchors.fill: parent
        data: _hiddenProperties._addressBarInternalInstance
    }

    Item {
        anchors.fill: parent
        HoverHandler {
            id: __addressBarHoverHandler
        }
    }
}
