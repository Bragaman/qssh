import QtQuick 2.5
import Ssh 1.0

Item {
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


    /*Ssh {
        id: ssh
        ptyCols: 80
        ptyRows: 24
        onConnected : {
           root.connected();
        }

        onShellRead : {
            root.shellRead(data);
        }

    }*/

    QMLTermWidget {
        id: terminal
        anchors.fill: parent
        font.family: "Monospace"
        font.pointSize: 12
        colorScheme: "cool-retro-term"
        session: QMLTermSession{
            id: mainsession
            initialWorkingDirectory: "$HOME"
            onMatchFound: {
                console.log("found at: %1 %2 %3 %4".arg(startColumn).arg(startLine).arg(endColumn).arg(endLine));
            }
            onNoMatchFound: {
                console.log("not found");
            }
        }
        onTerminalUsesMouseChanged: console.log(terminalUsesMouse);
        onTerminalSizeChanged: console.log(terminalSize);
        Component.onCompleted: mainsession.startShellProgram();

        QMLTermScrollbar {
            terminal: terminal
            width: 20
            Rectangle {
                opacity: 0.4
                anchors.margins: 5
                radius: width * 0.5
                anchors.fill: parent
            }
        }

    }


}


/*Rectangle {
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

         FontMetrics {
             id: fontMetrics
             font.family: "Courier New"
             font.pixelSize: 12
         }

         ListModel {
             id:charList
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
                console.debug(lineCount);

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

    Ssh {
        id: ssh
        ptyCols: 80//parseInt(parent.width / ssh.getFontWidth(textEdit.font.pixelSize))
        ptyRows: 24
        onConnected : {
           root.connected();
        }

        onShellRead : {
            root.shellRead(data);
            textEdit.text += data;
            textEdit.cursorPosition = textEdit.text.length
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
}*/

