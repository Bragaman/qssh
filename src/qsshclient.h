#ifndef QSSHCLIENT_H
#define QSSHCLIENT_H

#include "qssh_global.h"
#include <QObject>

class QSSHSHARED_EXPORT QSshKey{
 public:
    enum Type{
        UnknownType,
        Rsa,
        Dss,
        Rsa1,
        Ecdsa,
        Ed25519
    };
    QByteArray hash;
    QByteArray key;
    Type type;
};

class QSshProcess;
class QSshTcpSocket;
class QSshClientPrivate;

class QSSHSHARED_EXPORT QSshClient : public QObject
{
    Q_OBJECT
public:

    enum AuthenticationMethod {
        PasswordAuthentication,
        PublicKeyAuthentication,
        HostBasedAuthentication,
        InteractiveAuthentication
    };

    enum Error {
        ServerKnownChanged,
        ServerFoundOther,
        ServerNotKnown,
        ServerFileNotFound,
        ServerError,
        HostKeyInvalidError,
        AuthenticationError,
        ConnectionRefusedError,
        UnexpectedShutdownError,
        SocketError,
        UnknownError
    };

    enum STATE {
        STATE_NOT_CONNECTED = 0,
        STATE_TCP_CONNECTED = 2,
        STATE_SERVER_KNOWN = 3,
        STATE_AUTHENTICATE_MODE = 4,
        STATE_TRY_TO_AUTHENTICATE = 5,
        STATE_ACTIVATE_CHANNEL = 6
    };

    explicit QSshClient(QObject *parent = 0);
    ~QSshClient();

    void connectToHost(const QString & username,const QString & hostname,int port = 22);
    void disconnectFromHost ();
    void setPassphrase(const QString & pass);
    void setKeyFiles(const QString & publicKey,const QString & privateKey);

    bool loadKnownHosts(const QString & file);
    bool saveKnownHosts() const;
    bool addKnownHost();

    QSshKey hostKey() const;
    QString hostName() const;

    QSshProcess * openProcessChannel();
    QSshTcpSocket * openTcpSocket(const QString & hostName,quint16 port);

signals:
    void connected();
    void disconnected();
    void error (QSshClient::Error error);
    void authenticationRequired(QList<QSshClient::AuthenticationMethod> availableMethods);

private:
    QSshClientPrivate * sshClientPrivate;
    friend class QSshClientPrivate;
    friend class QSshChannelPrivate;
};

#endif // QSSHCLIENT_H
