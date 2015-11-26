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

#include "qsshchannel.h"
#include "qsshchannelprivate.h"
#include <QDebug>
#include <conio.h>
#include <QFile>

QSshChannel::QSshChannel(QSshClient * parent)
    :QObject(parent)
    ,d(new QSshChannelPrivate(this,parent)){
}

/*!
 * Destroys the QSshChannel object.
 */
QSshChannel::~QSshChannel()
{
}

QSshChannelPrivate::QSshChannelPrivate(QSshChannel *_p, QSshClient * c)
    :QThread(_p)
    ,p(_p)
    ,d_client(c)
    ,d_channel(0)
    ,d_session(d_client->sshClientPrivate->d_session)
    ,d_state(0)
    ,d_ptyCols(80)
    ,d_ptyRows(24)
{

}

QSshChannelPrivate::~QSshChannelPrivate()
{
    if(d_session){
        ssh_free(d_session);
    }
}


bool QSshChannelPrivate::activate(){
    //session
    if(d_state==1) {
        d_channel=ssh_channel_new(d_session);
        if(d_channel==NULL) {
            return SSH_ERROR;
        }
        int res = ssh_channel_open_session(d_channel);
        if(res == SSH_AGAIN) {
            return true;
        } else {
            d_state=2;
            return activate();
        }
    //transition to allow early cmd
    } else if (d_state==2){
        if(!d_next_actions.isEmpty()){
            d_state=d_next_actions.takeFirst();
            return activate();
        }else{
            return true;
        }

    //request pty
    } else if (d_state==3){
        qSshDebug() << d_cmd;
        int r = ssh_channel_request_exec(d_channel,qPrintable(d_cmd));
        if(r) {
            if(r==SSH_AGAIN){
                return true;
            }else{
                return false;
            }
        }
        d_state=66;
        start();
        return true;

    //start shell
    } else if (d_state==4){
        int r = ssh_channel_request_pty(d_channel);
        qSshDebug() << r;
        r = ssh_channel_change_pty_size(d_channel, d_ptyCols, d_ptyRows);
        qSshDebug() << r;
        r = ssh_channel_request_shell(d_channel);
        qSshDebug() << r;

        if(r != SSH_OK ){
            if(r==SSH_AGAIN){
                return true;
            }else{
                qWarning("QSshChannel: pty allocation failed");
                return false;
            }
        }
        d_state=67;
        start();
        return true;

    //start
    }  /*else if (d_state==4){
        int r=libssh2_channel_shell(d_channel);
        if(r){
            if(r==LIBSSH2_ERROR_EAGAIN){
                return true;
            }else{
                return false;
            }
        }
        p->setOpenMode(QIODevice::ReadWrite);
        d_state=9999;
        emit p->connected();
        return true;

    // tcp channel
    }else if (d_state==10){
        d_channel=libssh2_channel_direct_tcpip(d_session, qPrintable(d_host),d_port);
        if(d_channel==NULL){
            if(libssh2_session_last_error(d_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN) {
                return true;
            }else{
                return false;
            }
        }
        p->setOpenMode(QIODevice::ReadWrite);
        d_state=9999;
        return activate();

    //read channel
    }else if (d_state==9999){
        emit p->readyRead();
    }*/
    return true;
}


void QSshChannelPrivate::openSession(){
    if(d_state!=0){
        return;
    }
    d_state=1;
    activate();
}

void QSshChannelPrivate::startCmd(QString cmd){
    if(d_state>5){
        return;
    }

    d_cmd = cmd;

    if(d_state == 2) {
        d_state = 3;
        activate();
    } else {
        if(!d_next_actions.contains(3)) {
            d_next_actions.append(3);
        }
    }
}
void QSshChannelPrivate::startShell(){
    if(d_state>5){
        return;
    }

    if(d_state==2){
        d_state=4;
        activate();
    }else{
        if(!d_next_actions.contains(4))
            d_next_actions.append(4);
    }
}

void QSshChannelPrivate::changePtySize(int cols, int rows)
{
    d_ptyCols = cols;
    d_ptyRows = rows;
    qSshDebug() << d_ptyCols << d_ptyRows;
    ssh_channel_change_pty_size(d_channel, d_ptyCols, d_ptyRows);
}

void QSshChannelPrivate::writeOnShell(QString shell)
{
    if(d_channel) {
        int nwritten = ssh_channel_write(d_channel, shell.toLatin1(), shell.toLatin1().length());
        if (nwritten != shell.toLatin1().length()) {
            ssh_channel_send_eof(d_channel);
            ssh_channel_close(d_channel);
            ssh_channel_free(d_channel);
        }
    }
}



void QSshChannelPrivate::openTcpSocket(QString host,qint16 port){
    if(d_state!=0){
        return;
    }
    d_host=host;
    d_port=port;
    d_state=10;
    activate();
}

void QSshChannelPrivate::run()
{
    qSshDebug() << "run 11";

    if(d_state==66) {
        char buffer[256];
        int nbytes;
        nbytes = ssh_channel_read(d_channel, buffer, sizeof(buffer)-1, 0);
        if(nbytes>0) {
            buffer[255] = 0;
            emit d_client->channelCmdResponse(QString::fromLocal8Bit(buffer));
            memset(buffer, 0, 256);
        }
        while (nbytes > 0) {
            nbytes = ssh_channel_read(d_channel, buffer, sizeof(buffer)-1, 0);
            if(nbytes>0) {
                buffer[255] = 0;
                emit d_client->channelCmdResponse(QString::fromLocal8Bit(buffer));
                memset(buffer, 0, 256);
            }
        }

        d_state = 0;
        ssh_channel_send_eof(d_channel);
        ssh_channel_close(d_channel);
        ssh_channel_free(d_channel);
        emit d_client->channelEndCmdResponse();
    }

    if(d_state==67) {
        char buffer[256];
        int nbytes, nwritten;        
        while (ssh_channel_is_open(d_channel) && !ssh_channel_is_eof(d_channel)) {
            memset(buffer, 0, 256);
            nbytes = ssh_channel_read_nonblocking(d_channel, buffer, 255, 0);
            if (nbytes < 0)  {
                ssh_channel_send_eof(d_channel);
                ssh_channel_close(d_channel);
                ssh_channel_free(d_channel);
                d_channel = 0;
                break;
            }
            if (nbytes > 0) {
                emit d_client->channelShellResponse(QString::fromLocal8Bit(buffer));
                //d_emulator->parseCommand(buffer);
            }
            usleep(50000L); // 0.05 second

        }

        d_state = 0;
        emit d_client->disconnected();
        ssh_channel_send_eof(d_channel);
        ssh_channel_close(d_channel);
        ssh_channel_free(d_channel);
        d_channel = 0;
    }
    qSshDebug() << "leave thread";

}
