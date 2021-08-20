import QtQuick 2.15
import 'https://www.canonic.com/metonym/release/Metonym/' 0.1 as Metonym
import com.mycompany.qmlcomponents 1.0

Rectangle {
    id: root

    property alias theme: __internalThemedItem.theme

    Metonym.ThemedItem {
        id: __internalThemedItem
        inheritanceParent: root.parent
    }

    readonly property alias isSigningIn: __hidden_properties.isSigningIn
    property bool showSecretView: false
    color: showSecretView? Metonym.Styles.backgroundColor2: 'white'
    anchors.fill: parent

    QtObject {
        id: __hidden_properties
        property bool isSigningIn: false
    }

    Item {
        anchors {
            fill: parent
            margins: Metonym.Styles.popupPadding
        }

        Item {
            anchors.fill: parent

            Metonym.LineEdit {
                id: emailInput
                placeholder: "email"

                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }
            }

            Metonym.LineEdit {
                id: passwordInput
                placeholder: "password"

                anchors {
                    top: emailInput.bottom
                    right: parent.right
                    left: parent.left
                }
            }

            Metonym.Button {
                label: 'sign-in'

                anchors {
                    top: passwordInput.bottom
                    right: parent.right
                    left: parent.left
                }

                onClicked: {
                    const endpoint = "http://localhost:8080/auth"

                    var xhr = new XMLHttpRequest();

                    xhr.onload = function() {                        
                        if (xhr.status !== 200 && xhr.status !== 0) { // analyze HTTP status of the response
                            console.log(`Error ${xhr.status}: ${xhr.statusText}`); // e.g. 404: Not Found
                        }
                        else
                        {
                            var authToken = xhr.getResponseHeader("Auth-Token")

                            if(authToken)
                            {
                                let jwtComponents = authToken.split('.')
                                var header = JSON.parse(window.btoa(jwtComponents[0]))
                                var payload = JSON.parse(window.btoa(jwtComponents[1]))
                                var signiture = window.btoa(jwtComponents[2])

                                var userId = payload["userId"]
                                var displayName = payload["displayName"]
                                var identityProvider = payload["iss"]
                                var proofOfIdentityType = "jwt"
                                var proofOfIdentity = authToken
                                var avatarImgSource = Qt.resolvedUrl(payload["avatarImageSource"])

                                mainWindow.auth.signIn(userId,
                                                       displayName,
                                                       identityProvider,
                                                       proofOfIdentityType,
                                                       proofOfIdentity,
                                                       avatarImgSource)
                            }



                        }

                        __hidden_properties.isSigningIn = false
                    };

                    xhr.open("GET", endpoint);

                    //Send the proper header information along with the request
                    xhr.setRequestHeader('Content-Type', 'application/json');
                    xhr.setRequestHeader('Auth-Email', emailInput.text);
                    xhr.setRequestHeader('Auth-Password', passwordInput.text);

                    __hidden_properties.isSigningIn = true
                    xhr.send();
                }
            }
            visible: !mainWindow.auth.signedIn && root.showSecretView
        }

        Item {
            anchors.fill: parent
            Metonym.Button {
                label: 'Sign out'

                anchors {
                    centerIn: parent
                }

                onClicked: {
                    mainWindow.auth.signOut()
                }
            }
            visible: mainWindow.auth.signedIn && root.showSecretView
        }

        Column {
            id: disabledAuth
            anchors {
                centerIn: parent
                verticalCenterOffset: -20
            }

            Metonym.Icon {
                source: root.theme.icons.canonic
                color: '#cccccc'

                width: 80
                height: 80

                anchors {
                    horizontalCenter: disabledAuth.horizontalCenter
                }
            }

            Text {
                id: _text2
                anchors {
                    horizontalCenter: disabledAuth.horizontalCenter
                }

                font {
                    pointSize: 8
                    family: Metonym.Styles.monterratMedium.name
                }

                color: '#aaaaaa'
                text: 'Sorry but an invite is\nrequired to create an account'
                horizontalAlignment : Text.AlignHCenter
            }

            visible: !root.showSecretView
        }

        Timer {
            id: _secretTimer
            interval: 2000
            running: false
            repeat: false
            onTriggered: {
                if(__secretMouseArea.containsPress && __secretMouseArea.pressedButtons & Qt.MiddleButton)
                {
                    root.showSecretView = !root.showSecretView
                }
            }
        }

        MouseArea{
            id: __secretMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.MiddleButton
            onPressAndHold: _secretTimer.start()
            onReleased: _secretTimer.stop()
        }
    }
}
