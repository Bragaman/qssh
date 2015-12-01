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

#include "qsshprocess.h"
#include "qsshchannelprivate.h"

QSshProcess::QSshProcess(QSshClient * parent)
    :QSshChannel(parent)
{
    d->openSession();
    connect(this,SIGNAL(connected()),this,SIGNAL(started()));
}
/*!
 * Starts a login shell on the SSH server.
 *
 * If there is already a process running on this channel, the behavior is undefined.
 */
void QSshProcess::startShell(){
    d->startShell();
}

void QSshProcess::writeOnShell(QString shell)
{
    d->writeOnShell(shell);
}

void QSshProcess::changePtySize(int cols, int rows)
{
    d->changePtySize(cols, rows);
}
/*!
 * Invokes a shell command on the SSH server.
 *
 * If there is already a process running on this channel, the behavior is undefined.
 */
void QSshProcess::startCmd(const QString & cmd){
    d->startCmd(cmd);
}

void QSshProcess::sendData(const char *_data, int _len)
{
    QByteArray data(_data, _len);

    d->writeOnShell(data);
}
