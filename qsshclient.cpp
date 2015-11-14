#include "qsshchannelprivate.h"
#include "qsshprocess.h"
#include "qsshtcpsocket.h"
#include <QTimer>

QSshClient::QSshClient(QObject *parent) : QObject(parent),d(new QSshClientPrivate)
{
    d->p=this;
}

QSshClient::~QSshClient(){
    delete d;
}

void QSshClient::connectToHost(const QString &username, const QString &hostname, int port)
{
    d->d_hostName=hostname;
    d->d_userName=username;
    d->d_port=port;
    d->d_state=1;
    d->connectToHost(hostname,port);

}

void QSshClient::disconnectFromHost()
{
     d->d_reset();
}

void QSshClient::setPassphrase(const QString &pass)
{
    d->d_failedMethods.removeAll(QSshClient::PasswordAuthentication);
    d->d_failedMethods.removeAll(QSshClient::PublicKeyAuthentication);
    d->d_passphrase=pass;
    if(d->d_state>1){
       QTimer::singleShot(0,d,SLOT(d_readyRead()));
    }
}

void QSshClient::setKeyFiles(const QString &publicKey, const QString &privateKey)
{
    d->d_failedMethods.removeAll(QSshClient::PublicKeyAuthentication);
    d->d_publicKey=publicKey;
    d->d_privateKey=privateKey;
    if(d->d_state>1){
        QTimer::singleShot(0,d,SLOT(d_readyRead()));
    }
}

bool QSshClient::loadKnownHosts(const QString &file, QSshClient::KnownHostsFormat c)
{
    /*Q_UNUSED(c);
    return (libssh2_knownhost_readfile(d->d_knownHosts, qPrintable(file),
                                      LIBSSH2_KNOWNHOST_FILE_OPENSSH)==0);*/
}

bool QSshClient::saveKnownHosts(const QString &file, QSshClient::KnownHostsFormat c) const
{
    /*Q_UNUSED(c);
    return (libssh2_knownhost_writefile(d->d_knownHosts, qPrintable(file),
                                    LIBSSH2_KNOWNHOST_FILE_OPENSSH)==0);*/
}

bool QSshClient::addKnownHost(const QString &hostname, const QSshKey &key)
{
    /*int typemask=LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW;
        switch (key.type){
            case QSshKey::Dss:
                typemask|=LIBSSH2_KNOWNHOST_KEY_SSHDSS;
                break;
            case QSshKey::Rsa:
                typemask|=LIBSSH2_KNOWNHOST_KEY_SSHRSA;
                break;
            case QSshKey::UnknownType:
                return false;
        };


        return(libssh2_knownhost_add(d->d_knownHosts, qPrintable(hostname),
                                     NULL, key.key.data(), key.key.size(),
                                     typemask,  NULL));
*/
}

QSshKey QSshClient::hostKey() const
{
    return d->d_hostKey;
}

QString QSshClient::hostName() const
{
    return d->d_hostName;
}

QSshProcess * QSshClient::openProcessChannel(){
    if(d->d_state!=6){
        qWarning("cannot open channel before connected()");
        return NULL;
    }
    QSshProcess * s=new QSshProcess(this);
    d->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),d,SLOT(d_channelDestroyed()));
    return s;
}

QSshTcpSocket * QSshClient::openTcpSocket(const QString & hostName,quint16 port){
    if(d->d_state!=6){
        qWarning("cannot open channel before connected()");
        return NULL;
    }
    QSshTcpSocket * s=new QSshTcpSocket(this);
    d->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),d,SLOT(d_channelDestroyed()));
    s->d->openTcpSocket(hostName,port);
    return s;
}


static ssize_t qxt_p_libssh_recv(int socket,void *buffer, size_t length,int flags, void **abstract){
    Q_UNUSED(socket);
    Q_UNUSED(flags);
    QTcpSocket* c=reinterpret_cast<QTcpSocket*>(*abstract);
    int r=c->read(reinterpret_cast<char*>(buffer),length);
    if(r==0)
        return -EAGAIN;
    return r;
}

static ssize_t qxt_p_libssh_send(int socket,const void *buffer, size_t length,int flags, void **abstract){
    Q_UNUSED(socket);
    Q_UNUSED(flags);
    QTcpSocket* c=reinterpret_cast<QTcpSocket*>(*abstract);
    int r=c->write(reinterpret_cast<const char*>(buffer),length);
    if(r==0)
        return -EAGAIN;
    return r;
}


QSshClientPrivate::QSshClientPrivate()
    :d_session(0)
    //,d_knownHosts(0)
    ,d_state(0)
    ,d_errorCode(0)
{
    connect(this,SIGNAL(connected()),this,SLOT(d_connected()));
    connect(this,SIGNAL(disconnected()),this,SLOT(d_disconnected()));
    connect(this,SIGNAL(readyRead()),this,SLOT(d_readyRead()));

    d_session = ssh_new();

    d_state=0;
    d_errorCode=0;
    d_errorMessage=QString();
    d_failedMethods.clear();
    d_availableMethods.clear();

}

QSshClientPrivate::~QSshClientPrivate(){
    d_reset();
    if(d_session){
        ssh_disconnect(d_session);
        ssh_free(d_session);
        d_session = NULL;
    }
}

void QSshClientPrivate::d_connected(){
    d_state=2;
    qDebug() << "Tcp connected";
    d_readyRead();
}

void QSshClientPrivate::d_readyRead(){
    if(d_state==2){
        int sock = socketDescriptor();
        int ret=0;

        //1) initalise ssh session. exchange banner and stuff.

        if(ssh_options_set(d_session, SSH_OPTIONS_FD, &sock) < 0) {
            return;
        }

        if(ssh_options_set(d_session, SSH_OPTIONS_PORT, &d_port) < 0) {
            return;
        }

        if(ssh_options_set(d_session, SSH_OPTIONS_USER, d_userName.toLatin1()) < 0) {
            return;
        }

        if(ssh_options_set(d_session, SSH_OPTIONS_PORT, d_hostName.toLatin1()) < 0) {
            return;
        }

        if((ret = ssh_connect(d_session)) == SSH_AGAIN){
            return;
        }

        if (ret) {
            d_state = 0;
            qWarning("Failure establishing SSH session: %d", ret);
            emit p->error(QSshClient::UnexpectedShutdownError);
            d_reset();
            return;
        }

        //2) make sure remote is safe.
        int type;
        ssh_key key;
        unsigned char *hash = NULL;
        size_t len;
        QString fingerprint = "";
        if(ssh_get_publickey(d_session,&key) == SSH_OK) {
            if(ssh_get_publickey_hash(key, SSH_PUBLICKEY_HASH_MD5 , &hash, &len) == 0) {
                fingerprint = ssh_get_hexa(hash, len);
                d_hostKey.key = fingerprint.toLatin1();
                d_hostKey.hash = (const char *)hash;
                type = ssh_key_type(key);
                ssh_key_free(key);
            }
        }
        qDebug() << "Key type" << type;
        switch (type){
            case SSH_KEYTYPE_RSA:
                d_hostKey.type=QSshKey::Rsa;
                break;
            case SSH_KEYTYPE_RSA1:
                d_hostKey.type=QSshKey::Rsa1;
                break;
            case SSH_KEYTYPE_DSS:
                d_hostKey.type=QSshKey::Dss;
                break;
            case SSH_KEYTYPE_ECDSA:
                d_hostKey.type=QSshKey::Ecdsa;
                break;
            case SSH_KEYTYPE_ED25519:
                d_hostKey.type=QSshKey::Ed25519;
                break;
            default:
                d_hostKey.type=QSshKey::UnknownType;
        }

        if(fingerprint != "") {

            int state = ssh_is_server_known(d_session);
// for test
state = SSH_SERVER_KNOWN_OK;
            switch(state){
                case SSH_SERVER_KNOWN_OK:
                    d_state=3;
                    d_readyRead();
                    return;
                case SSH_SERVER_KNOWN_CHANGED:
                    d_delaydError=QSshClient::ServerKnownChanged;
                    break;
                case SSH_SERVER_FOUND_OTHER:
                    d_delaydError=QSshClient::ServerFoundOther;
                    break;
                case SSH_SERVER_NOT_KNOWN:
                    d_delaydError=QSshClient::ServerNotKnown;
                    break;
                case SSH_SERVER_FILE_NOT_FOUND:
                    d_delaydError=QSshClient::ServerFileNotFound;
                    break;
                case SSH_SERVER_ERROR:
                    d_delaydError=QSshClient::ServerError;
                    break;
            }
        }else{
            d_delaydError=QSshClient::HostKeyInvalidError;
        }

        qDebug() << d_delaydError;

        d_getLastError();
        d_reset();
        disconnectFromHost ();
        QTimer::singleShot(0,this,SLOT(d_delaydErrorEmit()));
        return;

    } else if(d_state==3){
        //3) try auth type "none" and get a list of other methods
        //   in the likely case that the server doesnt like "none"

        QByteArray username=d_userName.toLatin1();
qDebug() << username;
        int rc = ssh_userauth_none(d_session, NULL);

        switch(rc) {
            case SSH_AUTH_ERROR:
                qDebug() << "SSH_AUTH_ERROR";
                break;
            case SSH_AUTH_DENIED:
                qDebug() << "SSH_AUTH_DENIED";
                break;
            case SSH_AUTH_PARTIAL:
                qDebug() << "SSH_AUTH_DENIED";
                break;
            case SSH_AUTH_SUCCESS:
                qDebug() << "SSH_AUTH_SUCCESS";
                break;
            case SSH_AUTH_AGAIN:
                qDebug() << "SSH_AUTH_AGAIN";
                break;
        }

        int authList = ssh_userauth_list(d_session, NULL);

        if(authList & SSH_AUTH_METHOD_PASSWORD) {
            qDebug() << "SSH_AUTH_METHOD_PASSWORD allowed";
            d_availableMethods<<QSshClient::PasswordAuthentication;
        }
        if(authList & SSH_AUTH_METHOD_PUBLICKEY) {
            qDebug() << "SSH_AUTH_METHOD_PUBLICKEY allowed";
            d_availableMethods<<QSshClient::PublicKeyAuthentication;

        }
        if(authList & SSH_AUTH_METHOD_HOSTBASED) {
            qDebug() << "SSH_AUTH_METHOD_HOSTBASED allowed";
            d_availableMethods<<QSshClient::HostBasedAuthentication;
        }
        if(authList & SSH_AUTH_METHOD_INTERACTIVE) {
            qDebug() << "SSH_AUTH_METHOD_INTERACTIVE allowed";
            d_availableMethods<<QSshClient::InteractiveAuthentication;
        }

        if(authList == 0){
            if(rc == SSH_AUTH_SUCCESS){
                //null auth ok
                emit p->connected();
                d_state=5;
                return;
            }else if(rc==SSH_AUTH_AGAIN) {
                return;
            }else{
                d_getLastError();
                emit p->error(QSshClient::UnexpectedShutdownError);
                d_reset();
                emit p->disconnected();
                return;
            }
        }

        d_state=4;
        d_readyRead();
    } else if(d_state==4) {
        if(d_availableMethods.contains(QSshClient::PublicKeyAuthentication) &&
           !d_privateKey.isNull() &&
           !d_failedMethods.contains(QSshClient::PublicKeyAuthentication)){

            qDebug() << "auth by key";
            d_currentAuthTry=QSshClient::PublicKeyAuthentication;
            d_state=5;
            d_readyRead();
            return;
        }
        if(d_availableMethods.contains(QSshClient::PasswordAuthentication) &&
           !d_passphrase.isNull() &&
           !d_failedMethods.contains(QSshClient::PasswordAuthentication)){
            qDebug() << "auth by password";
            d_currentAuthTry=QSshClient::PasswordAuthentication;
            d_state=5;
            d_readyRead();
            return;
        }

        qDebug() << "auth required";

        emit p->authenticationRequired(d_availableMethods);
    } else if(d_state==5) {
        int ret(0);
        if(d_currentAuthTry==QSshClient::PasswordAuthentication){
            ret = ssh_userauth_password(d_session, qPrintable(d_userName),
                                          qPrintable(d_passphrase));

            qDebug() << ret << "password";
        }else if(d_currentAuthTry==QSshClient::PublicKeyAuthentication){
            ret = ssh_userauth_publickey_auto(d_session,
                                               qPrintable(d_userName),
                                               qPrintable(d_passphrase));
        }
        if(ret == SSH_AGAIN ){
            return;
        }else if(ret==0){
            d_state=6;
            qDebug() << "Connected";
            emit p->connected();
        }else{
            d_getLastError();
            emit p->error(QSshClient::AuthenticationError);
            d_failedMethods.append(d_currentAuthTry);
            d_state=4;
            d_readyRead();
        }
    }else if(d_state==6){
        QList<QSshChannel*>::const_iterator i;
        for (i = d_channels.constBegin(); i != d_channels.constEnd(); ++i){
            bool ret=(*i)->d->activate();
            if(!ret){
                d_getLastError();
            }
        }
    }else{
        qDebug("did not expect to receive data in this state");
    }
}

void QSshClientPrivate::d_reset(){
    //teardown
    /*if(d_knownHosts){
        libssh2_knownhost_free(d_knownHosts);
    }*/

    if(d_state>1){
        ssh_disconnect(d_session);
    }

    if(d_session){
        ssh_free(d_session);
    }

    d_state=0;
    d_errorCode=0;
    d_errorMessage=QString();
    d_failedMethods.clear();
    d_availableMethods.clear();


    //buildup
    /*d_session = libssh2_session_init_ex(NULL,NULL,NULL,reinterpret_cast<void*>(this));
    libssh2_session_callback_set(d_session,LIBSSH2_CALLBACK_RECV,reinterpret_cast<void*>(&qxt_p_libssh_recv));
    libssh2_session_callback_set(d_session,LIBSSH2_CALLBACK_SEND,reinterpret_cast<void*>(&qxt_p_libssh_send));
    Q_ASSERT(d_session);

    d_knownHosts= libssh2_knownhost_init(d_session);
    Q_ASSERT(d_knownHosts);

    libssh2_session_set_blocking(d_session,0);*/


}

void QSshClientPrivate::d_disconnected (){
    if(d_state!=0){
        qWarning("unexpected shutdown");
        d_reset();
    }
}

void QSshClientPrivate::d_getLastError(){
    const char * msg;
    d_errorCode = ssh_get_error_code(d_session );
    msg = ssh_get_error(d_session);
    d_errorMessage=QString::fromLocal8Bit(msg);
}



void QSshClientPrivate::d_channelDestroyed(){
    QSshChannel* channel=  qobject_cast<QSshChannel*>(sender());
    d_channels.removeAll(channel);
}

void QSshClientPrivate::d_delaydErrorEmit(){
    emit p->error(d_delaydError);
}
