import QtQuick 2.0
import Ssh 1.0

Rectangle {
    id:root

    property alias host: ssh.host
    property alias port: ssh.port
    property alias username: ssh.username
    property alias passphrase: ssh.passphrase

    signal connected();
    signal shellRead(string data);

    function connect() {
        ssh.connectToHost();
    }

    color:"black"

    Flickable {
         id: flick

         anchors.fill: parent
         contentWidth: textEdit.paintedWidth
         contentHeight: textEdit.paintedHeight
         clip: true
         flickableDirection: Flickable.VerticalFlick

         function ensureVisible(r)
         {
             if (contentX >= r.x)
                 contentX = r.x;
             else if (contentX+width <= r.x+r.width)
                 contentX = r.x+r.width-width;
             if (contentY >= r.y)
                 contentY = r.y;
             else if (contentY+height <= r.y+r.height)
                 contentY = r.y+r.height-height;
         }

         TextEdit {
            id:textEdit
            color: "#FFFFFF"
            font.family: "Courier New"
            opacity: 0.8
            font.pixelSize: 12
            textFormat: TextEdit.AutoText
            width: flick.width
            height: flick.height
            focus: true
            wrapMode: TextEdit.Wrap
            onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)


            Keys.onPressed: {
                ssh.writeOnShell(event.text);
                event.accepted = true;
            }

            Ssh {
                id: ssh
                onConnected : {
                   root.connected();
                }
                onShellRead : {
                    root.shellRead(data);
                    textEdit.text += data;
                    textEdit.cursorPosition = textEdit.text.length
                }
            }
         }

        // Only show the scrollbars when the view is moving.
        states: State {
           name: "ShowBars"
           when: flick.movingVertically
           PropertyChanges { target: verticalScrollBar; opacity: 1 }
        }

        transitions: Transition {
           NumberAnimation { properties: "opacity"; duration: 400 }
        }
    }

    ScrollBar {
        id: verticalScrollBar
        width: 12
        height: flick.height
        anchors.right: flick.right
        anchors.top: flick.top
        opacity: 0
        orientation: Qt.Vertical
        position: flick.visibleArea.yPosition < 0 ? 0 : flick.visibleArea.yPosition
        pageSize: flick.visibleArea.heightRatio
    }
}

