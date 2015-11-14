#include "qsshclient.h"
#include "qsshchannel.h"
#include <QTcpSocket>

extern "C"{
    #include <libssh/libssh.h>
    #include <errno.h>
}

class QSSHSHARED_EXPORT QSshClientPrivate : public QTcpSocket{
    Q_OBJECT
public:
    QSshClientPrivate();
    ~QSshClientPrivate();
    void d_reset();
    void d_getLastError();

    QSshClient * p;
    ssh_session d_session;
//    LIBSSH2_KNOWNHOSTS * d_knownHosts;
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
    void d_connected();
    void d_disconnected();
    void d_channelDestroyed();
    void d_delaydErrorEmit();
};

