/*
 * This file is part of the QSsh Library
 *
 * Copyright (c) 2015 by Gyger Jean-Luc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 *
 */



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

    enum STATE {
        STATE_NOT_CONNECTED = 0,
        STATE_TCP_CONNECTED = 2,
        STATE_SERVER_KNOWN = 3,
        STATE_AUTHENTICATE_MODE = 4,
        STATE_TRY_TO_AUTHENTICATE = 5,
        STATE_ACTIVATE_CHANNEL = 6
    };

    enum RES {
        QSSH_OK = 0,
        QSSH_ERROR = -1,
        QSSH_AGAIN = -2,
        QSSH_EOF = -127,
        QSSH_SERVER_ERROR = -1000,
        QSSH_SERVER_NOT_KNOWN = -1001,
        QSSH_SERVER_KNOWN_OK = -1002,
        QSSH_SERVER_KNOWN_CHANGED = -1003,
        QSSH_SERVER_FOUND_OTHER = -1004,
        QSSH_SERVER_FILE_NOT_FOUND = -1004
    };

    explicit QSshClient(QObject *parent = 0);
    ~QSshClient();

    void connectToHost(const QString & username,const QString & hostname,int port = 22);
    void disconnectFromHost ();
    void setPassphrase(const QString & pass);
    void setKeyFiles(const QString & publicKey,const QString & privateKey);

    bool loadKnownHosts(const QString & file);
    bool addKnownHost();

    QSshKey hostKey() const;
    QString hostName() const;

    QSshProcess * openProcessChannel();
    QSshTcpSocket * openTcpSocket(const QString & hostName,quint16 port);

signals:
    void connected();
    void disconnected();
    void error (int error, QString msg);
    void authenticationRequired(QList<QSshClient::AuthenticationMethod> availableMethods);
    void channelCmdResponse(QString);
    void channelEndCmdResponse();
    void channelShellResponse(QString);

private:
    QSshClientPrivate * sshClientPrivate;
    friend class QSshClientPrivate;
    friend class QSshChannelPrivate;
};

#endif // QSSHCLIENT_H
