#include "qsshchannel.h"
#include "qsshchannelprivate.h"

QSshChannel::QSshChannel(QSshClient * parent)
    :QIODevice(parent)
    ,d(new QSshChannelPrivate(this,parent)){
}

/*!
 * Destroys the QSshChannel object.
 */
QSshChannel::~QSshChannel()
{
}

QSshChannelPrivate::QSshChannelPrivate(QSshChannel *_p, QSshClient * c)
    :QObject(0)
    ,p(_p)
    ,d_client(c)
    ,d_channel(0)
    ,d_session(d_client->d->d_session)
    ,d_read_stream_id(0)
    ,d_write_stream_id(0)
    ,d_state(0)
{
}

/*!
    \reimp
*/
qint64 QSshChannel::readData(char* buff, qint64 len) {
    ssize_t ret=ssh_channel_read(d->d_channel, buff, len, 0);
    if(ret<0){
        if(ret==SSH_AGAIN){
            return 0;
        }else{
            return -1;
        }
    }
    return ret;
}

/*!
    \reimp
*/
qint64 QSshChannel::writeData(const char* buff, qint64 len){
    ssize_t ret=ssh_channel_write(d->d_channel, buff, len);
    if(ret<0){
        if(ret==SSH_AGAIN){
            return 0;
        }else{
            return -1;
        }
    }
    return ret;
}
/*!
 * \reimp
 */
bool QSshChannel::isSequential () const{
    return true;
}

bool QSshChannelPrivate::activate(){
    //session
    /*if(d_state==1){
        d_channel=ssh_channel_new(d_session);
        if(d_channel==NULL){
            if(libssh2_session_last_error(d_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN) {
                return true;
            }else{
                return false;
            }
        }
        d_state=2;
        return activate();

    //transition to allow early cmd
    }else if (d_state==2){
        if(!d_next_actions.isEmpty()){
            d_state=d_next_actions.takeFirst();
            return activate();
        }else{
            return true;
        }

    //request pty
    }else if (d_state==5){
        int r=libssh2_channel_request_pty(d_channel,d_pty.data());
        if(r){
            if(r==LIBSSH2_ERROR_EAGAIN){
                return true;
            }else{
                qWarning("QSshChannel: pty allocation failed");
                return false;
            }
        }
        d_state=2;
        return activate();

    //start
    }else if (d_state==3){
        int r=libssh2_channel_exec(d_channel,qPrintable(d_cmd));
        if(r){
            if(r==LIBSSH2_ERROR_EAGAIN){
                return true;
            }else{
                return false;
            }
        }
        p->setOpenMode(QIODevice::ReadWrite);
        d_state=66;
        emit p->connected();
        return true;

    //start shell
    }else if (d_state==4){
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

void QSshChannelPrivate::start(QString cmd){
    if(d_state>5){
        return;
    }
    d_cmd=cmd;
    if(d_state==2){
        d_state=3;
        activate();
    }else{
        if(!d_next_actions.contains(3))
            d_next_actions.append(3);
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
