#include "qsshchannel.h"
#include "qsshchannelprivate.h"
#include <QDebug>

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
{
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
    } else if (d_state==5){
        int r=ssh_channel_request_pty(d_channel);
        if(r != SSH_OK ){
            if(r==SSH_AGAIN){
                return true;
            }else{
                qWarning("QSshChannel: pty allocation failed");
                return false;
            }
        }
        d_state=2;
        return activate();

    //start
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
    } /*else if (d_state==4){
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

void QSshChannelPrivate::requestPty(QByteArray pty){
    if(d_state>5){
        return;
    }
    d_pty=pty;
    if(d_state==2){
        d_state=5;
        activate();
    }else{
        if(!d_next_actions.contains(5))
            d_next_actions.append(5);
    }
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
    qSshDebug() << "run";
    char buffer[256];
    int nbytes;
    if(d_state==66) {
          nbytes = ssh_channel_read(d_channel, buffer, sizeof(buffer), 0);
          qSshDebug() << buffer;
          while (nbytes > 0)
          {
            if (write(1, buffer, nbytes) != (unsigned int) nbytes){
                // error
            }
            nbytes = ssh_channel_read(d_channel, buffer, sizeof(buffer), 0);
            qSshDebug() << buffer;

          }
    }

    qSshDebug() << "leave thread";
}
