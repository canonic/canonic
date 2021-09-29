import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym

Item {
    id: root

    Item {
        id: hiddenProperties
    }

    Rectangle {
        anchors.fill: parent
        color: Metonym.Styles.tableViewColor
    }

    Component {
        id: jsonObjectComponent

        Column {
            id: __columnContainer
            spacing: 5

            property var jsonObject: _jsonObject

            property var objectKeys: Object.keys(jsonObject);
            property var widestKey: {
                // 200 is the arbitary default width
                let _widestKey = 200

                const itemCount = __objectKeysRepeater.count

                for(var i = 0; i < itemCount; i++)
                {
                    const objectKeyItem = __objectKeysRepeater.itemAt(i)
                    // objectKeyItem can be null if loading asynchronously
                    if(objectKeyItem !== null && _widestKey < objectKeyItem.keyWidth)
                    {
                        _widestKey = objectKeyItem.keyWidth
                    }
                }
                return _widestKey
            }



            Repeater {
                id: __objectKeysRepeater

                model: objectKeys.length

                delegate: Column {
                    id: __objectColumn
                    property var objectkey: __columnContainer.objectKeys[model.index]
                    property var objectValue: jsonObject[objectkey]
                    property alias keyWidth: __keyText.contentWidth
                    property bool isOpen: true

                    spacing: __columnContainer.spacing

                    Row {
                        spacing: 20

                        Row {
                            spacing: 5

                            Metonym.Icon
                            {
                                width: 14
                                height: 14
                                color: {
                                    if(__iconMouseArea.containsMouse)
                                    {
                                        if(__iconMouseArea.containsPress)
                                        {
                                            return Metonym.Styles.buttonColorDown
                                        }
                                        return Metonym.Styles.buttonColorHover
                                    }
                                    return Metonym.Styles.buttonColorDefault
                                }

                                source: {
                                    if(__valueText.visible)
                                    {
                                        return ''
                                    }

                                    if(__objectColumn.isOpen)
                                    {
                                        return 'qrc:/assets/icons/caret-down-size-20px.svg'
                                    }

                                    return 'qrc:/assets/icons/caret-right-size-20px.svg'
                                }

                                MouseArea {
                                    id: __iconMouseArea
                                    hoverEnabled: true
                                    anchors.fill: parent

                                    onClicked: __objectColumn.isOpen = !__objectColumn.isOpen
                                }
                            }

                            TextInput {
                                id: __keyText
                                width: __columnContainer.widestKey
                                text: objectkey + ':'
                                color: 'white'
                                font{
                                    family: Metonym.Styles.monterratMedium.name
                                    pointSize: 9
                                }
                                readOnly: true
                                selectByMouse: true
                            }
                        }
                        TextInput {
                            id: __valueText
                            text: JSON.stringify(objectValue)
                            color: 'white'
                            font{
                                family: Metonym.Styles.monterratMedium.name
                                pointSize: 9
                            }
                            readOnly: true
                            selectByMouse: true

                            visible: {
                                if(objectValue === null)
                                {
                                    return true
                                }

                                if(Array.isArray(objectValue) && !objectValue.length)
                                {
                                    return true
                                }

                                if(typeof objectValue !== 'object')
                                {
                                    return true
                                }

                                return false
                            }
                        }
                    }

                    Loader {
                        visible: {
                            if(!__objectColumn.isOpen)
                            {
                                return false
                            }

                            if(typeof objectValue === 'object' && objectValue !== null )
                            {
                                return true
                            }

                            if(Array.isArray(objectValue) && objectValue.length > 0)
                            {
                                return true
                            }

                            return false
                        }

                        active: visible

                        property var _jsonObject: {
                            if(Array.isArray(objectValue) && objectValue.length > 0)
                            {
                                // Convert the array into an object
                                let arrayObject = {}
                                for(var i = 0; i< objectValue.length; i++)
                                {
                                    arrayObject[i] = objectValue[i]
                                }
                                return arrayObject
                            }

                            return objectValue
                        }

                        x: 20

                        sourceComponent: jsonObjectComponent
                    }
                }
            }
        }
    }

    Flickable {
        anchors.fill: parent

        contentWidth: __content.width
        contentHeight: __content.height

        Item {
            id: __content
            width: root.width
            height: _topLevelJsonLoader.height

            Loader {
                id: _topLevelJsonLoader
                property var _jsonObject: JSON.parse(window.document.rawData)
                sourceComponent: jsonObjectComponent
            }
        }

        Metonym.ScrollBar.vertical: Metonym.ScrollBar {}
    }
}
