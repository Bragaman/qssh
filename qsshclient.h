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

    enum AuthenticationMethod{
        PasswordAuthentication,
        PublicKeyAuthentication,
        HostBasedAuthentication,
        InteractiveAuthentication
    };

    enum KnownHostsFormat{
        OpenSslFormat
    };

    enum Error{
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


    explicit QSshClient(QObject *parent = 0);
    ~QSshClient();

    void connectToHost(const QString & username,const QString & hostname,int port = 22);
    void disconnectFromHost ();
    void setPassphrase(const QString & pass);
    void setKeyFiles(const QString & publicKey,const QString & privateKey);

    bool loadKnownHosts(const QString & file, KnownHostsFormat c = OpenSslFormat);
    bool saveKnownHosts(const QString & file, KnownHostsFormat c = OpenSslFormat) const;
    bool addKnownHost(const QString & hostname,const QSshKey & key);

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
    QSshClientPrivate * d;
    friend class QSshClientPrivate;
    friend class QSshChannelPrivate;
};

#endif // QSSHCLIENT_H
