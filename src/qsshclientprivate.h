#include "qsshclient.h"
#include "qsshchannel.h"
#include <QObject>

extern "C"{
    #include <libssh/libssh.h>
    #include <errno.h>
}

class QSSHSHARED_EXPORT QSshClientPrivate : public QObject {
    Q_OBJECT
public:
    QSshClientPrivate();
    ~QSshClientPrivate();
    void d_reset();
    void d_getLastError();

    QSshClient * sshClient;
    ssh_session d_session;
    int d_state;
    QString d_hostName;
    int d_port;
    QSshKey  d_hostKey;
    QString d_userName;
    QString d_passphrase;
    QString d_privateKey;
    QString d_publicKey;
    QString d_errorMessage;
    int d_errorCode;
    QSshClient::Error d_delaydError;
    QList<QSshClient::AuthenticationMethod> d_availableMethods;
    QList<QSshClient::AuthenticationMethod> d_failedMethods;
    QSshClient::AuthenticationMethod d_currentAuthTry;

    QList<QSshChannel*> d_channels;
public slots:
    void d_readyRead();
    void d_disconnected();
    void d_channelDestroyed();
    void d_delaydErrorEmit();
};

