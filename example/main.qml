import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Ssh 1.0


ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "WinSshConsole" + (terminal.title !== "" ? " - " + terminal.title : "" )

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: {
                    loginDialog.visible = true;
                }
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    SshTerminal {
        id: terminal
        anchors.fill: parent
        onFinished : {
            console.debug("FINISHED");
            Qt.quit();
        }
        onServerNotKnown: {
            console.debug(message);
            sshDialog.title = qsTr("Ssh error");
            sshDialog.text = message;
            sshDialog.visible = true;
        }
    }


    MessageDialog {
        id: sshDialog
        visible:false;
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            terminal.acceptUnkownHost();
        }
        onNo: {
            Qt.quit();
        }
    }

    Dialog {
        id:loginDialog
        visible: false
        title: "Login"
        width: 400
        height: 150
        standardButtons: StandardButton.Open | StandardButton.Cancel

        onAccepted:{
            terminal.host = host.text;
            terminal.passphrase = password.text;
            terminal.port = port.text;
            terminal.username = username.text;
            terminal.startShellProgramm();
        }

        Grid {
            width: parent.width
            height: parent.height
            columns: 2
            rows: 3
            rowSpacing: 10

            Label {
                text: qsTr("Host/port:")
                width: parent.width * 1 / 3
                height: 20
            }

            Item {
                width: parent.width * 2 / 3
                height: 20
                Row {
                    width: parent.width
                    height: parent.height
                    TextField {
                        id:host
                        width: (parent.width * 2 / 3) - 10
                        height: parent.height
                    }

                    Text {
                        text: ":"
                        horizontalAlignment: Text.AlignHCenter
                        width: 10
                        height: parent.height
                    }

                    TextField {
                        id:port
                        width: (parent.width * 1 / 3) - 10
                        height: parent.height
                        text:"22"
                    }
                }
            }


            Label {
                text: qsTr("Username:")
                width: parent.width  * 1 / 3
                height: 20
            }

            TextField {
                id:username
                width: parent.width  * 2 / 3
                height: 20
            }

            Label {
                text: qsTr("Password:")
                width: parent.width  * 1 / 3
                height: 20
            }

            TextField {
                id:password
                echoMode: TextInput.Password
                width: parent.width * 2 / 3
                height: 20
            }

        }
    }
}

