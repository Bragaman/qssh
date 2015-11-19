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

    TextEdit {
        id:textEdit
        color: "#FFFFFF"
        opacity: 0.8
        font.pixelSize: 12
        anchors.fill: parent
        textFormat: TextEdit.AutoText

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
}

