import QtQuick 2.0
import Ssh 1.0

TextEdit {
    id:root
    property alias host: ssh.host
    property alias port: ssh.port
    property alias username: ssh.username
    property alias passphrase: ssh.passphrase

    textFormat: TextEdit.AutoText

    signal connected();
    signal shellRead(string data);
    function connect() {
        ssh.connectToHost();
    }

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
            root.text += data;
            root.cursorPosition = root.text.length
        }
    }


}

