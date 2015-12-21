import QtQuick 2.5
import Ssh 1.0

Item {
    id:root

    property alias host: mainsession.host
    property alias port: mainsession.port
    property alias username: mainsession.username
    property alias passphrase: mainsession.passphrase
    property alias title: mainsession.title

    signal finished();
    signal error(int code, string message);
    signal serverNotKnown(string message);
    signal connected();

    function acceptUnkownHost() {
        mainsession.acceptUnkownHost();
    }

    function startShellProgramm() {
        mainsession.startShellProgram();
    }

    function close() {
        mainsession.close();
    }

    QMLTermWidget {
        id: ssh
        anchors.fill: parent
        font.family: "Courier New"
        font.pointSize: 10
        colorScheme: "cool-retro-term"
        focus:true;
        session: QMLTermSession{
            id: mainsession
            onMatchFound: {
                console.log("found at: %1 %2 %3 %4".arg(startColumn).arg(startLine).arg(endColumn).arg(endLine));
            }
            onNoMatchFound: {
                console.log("not found");
            }
            onFinished: {
                root.finished();
            }

            onStarted: {
                root.connected();
            }

            onError: {
                console.debug(code + "/" + message);
                root.error(code, message);
                if(code == -1001) { // @TODO, give constant available in QML
                    root.serverNotKnown(message);
                }
            }
        }
        onTerminalUsesMouseChanged: console.log(terminalUsesMouse);
        onMouseSignal: {
            console.debug("Mouse signal:" + button)
            focus = true;
        }

        onTerminalSizeChanged: console.log(terminalSize);

        QMLTermScrollbar {
            terminal: ssh
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
