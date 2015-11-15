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

#include <QThread>
#include "qsshclient.h"
#include "qsshclientprivate.h"
#include "qsshchannel.h"

class QSshChannelPrivate : public QThread {
    Q_OBJECT
public:
    QSshChannelPrivate(QSshChannel*,QSshClient *);
    ~QSshChannelPrivate();

    QSshChannel * p;
    QSshClient * d_client;
    ssh_channel  d_channel;
    ssh_session  d_session;

    int d_state;
    bool activate();

    QList<int> d_next_actions;
    QString d_cmd;
    QByteArray d_pty;
    void openSession();
    void requestPty(QByteArray pty);
    void startCmd(QString cmd);
    void startShell();
    void writeOnShell(QString shell);

    void openTcpSocket(QString host,qint16 port);
    QString d_host;
    qint16 d_port;

protected:
    void run();
};

