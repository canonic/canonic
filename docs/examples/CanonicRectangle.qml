// CanonicRectangle.qml
import QtQuick

Rectangle {
    // Give this element the id: root
    id: root

    // Bind the properties width and height: <name>: <value>
    width: 120
    height: 240

    // Set the background color of the Rectangle
    color: "cyan"

    // Declare a nested element (child of root)
    Image {
        id: image

        // Position the image in the horizontal centre of the 
        // parent element using aboslute positioning
        x: (parent.width - width)/2
        
        // Position the image in the vertical center of the
        // parent element using anchors
        anchors.verticalCenter: parent.verticalCenter
        
        fillMode: Image.PreserveAspectFit
        
        width: 60
        height: 60
        
        // Load an image from a url
        source: 'https://www.canonic.com/assets/images/favicon.png'
    }

    // Another child of root
    Text {
        // un-named element
        
        // Position the text so that it's top is anchored to
        // the bottom of the image element
        anchors.top: image.bottom
        
        anchors.horizontalCenter: image.horizontalCenter
        
        text: 'Canonic'
    }
}