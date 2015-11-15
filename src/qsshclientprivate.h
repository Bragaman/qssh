/*
 * This file is part of the QSsh Library
 *
 * Copyright (c) 2015 by Gyger Jean-Luc
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "qsshclient.h"
#include "qsshchannel.h"
#include <QObject>

extern "C"{
    #include <libssh/libssh.h>
    #include <errno.h>
}

class QSSHSHARED_EXPORT QSshClientPrivate : public QObject {
    Q_OBJECT

    enum RES {
        QSSH_OK = SSH_OK,
        QSSH_ERROR = SSH_ERROR
    };

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
    QList<QSshClient::AuthenticationMethod> d_availableMethods;
    QList<QSshClient::AuthenticationMethod> d_failedMethods;
    QSshClient::AuthenticationMethod d_currentAuthTry;

    QList<QSshChannel*> d_channels;
public slots:
    void d_readyRead();
    void d_disconnected();
    void d_channelDestroyed();    
};

