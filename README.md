# qssh

##Description
This project is a QML port of qmltermwidget (https://github.com/Swordfish90/qmltermwidget), for Windows which allow to open an ssh connection. 
To open the ssh connection, QSsh use the library libssh (https://www.libssh.org/).

It is currently not fully usable but can be tested.

##Example
```
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Ssh 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "test"
    SshTerminal {
        id: terminal
        anchors.fill: parent
        host: "yoursshserver.com"
        username: "username"
        passphrase: "password"
        onFinished : {
            console.debug("FINISHED");
        }
    }
}
```
