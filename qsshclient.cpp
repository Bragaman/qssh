#include "qsshchannelprivate.h"
#include "qsshprocess.h"
#include "qsshtcpsocket.h"
#include <QTimer>


QSshClient::QSshClient(QObject *parent) : QObject(parent), sshClientPrivate(new QSshClientPrivate)
{
    sshClientPrivate->sshClient = this;
}

QSshClient::~QSshClient() {
    delete sshClientPrivate;
}

void QSshClient::connectToHost(const QString &username, const QString &hostname, int port)
{
    sshClientPrivate->d_hostName = hostname;
    sshClientPrivate->d_userName = username;
    sshClientPrivate->d_port = port;
    sshClientPrivate->d_state = STATE_TCP_CONNECTED;
    sshClientPrivate->d_readyRead();

}

void QSshClient::setPassphrase(const QString &pass)
{
    sshClientPrivate->d_failedMethods.removeAll(QSshClient::PasswordAuthentication);
    sshClientPrivate->d_failedMethods.removeAll(QSshClient::PublicKeyAuthentication);
    sshClientPrivate->d_passphrase=pass;
    if(sshClientPrivate->d_state > STATE_TCP_CONNECTED){
        QTimer::singleShot(0, sshClientPrivate, SLOT(d_readyRead()));
    }
}

void QSshClient::setKeyFiles(const QString &publicKey, const QString &privateKey)
{
    sshClientPrivate->d_failedMethods.removeAll(QSshClient::PublicKeyAuthentication);
    sshClientPrivate->d_publicKey = publicKey;
    sshClientPrivate->d_privateKey = privateKey;
    if(sshClientPrivate->d_state > STATE_TCP_CONNECTED){
        QTimer::singleShot(0,sshClientPrivate,SLOT(d_readyRead()));
    }
}

bool QSshClient::loadKnownHosts(const QString &file)
{
    return ssh_options_set(sshClientPrivate->d_session, SSH_OPTIONS_KNOWNHOSTS, file.toLatin1());
}

bool QSshClient::addKnownHost()
{
    return ssh_write_knownhost(sshClientPrivate->d_session);
}

QSshKey QSshClient::hostKey() const
{
    return sshClientPrivate->d_hostKey;
}

QString QSshClient::hostName() const
{
    return sshClientPrivate->d_hostName;
}

QSshProcess * QSshClient::openProcessChannel(){
    if(sshClientPrivate->d_state != QSshClient::STATE_ACTIVATE_CHANNEL){
        qSshDebug("cannot open channel before connected()");
        return NULL;
    }

    QSshProcess * s = new QSshProcess(this);
    sshClientPrivate->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),sshClientPrivate,SLOT(d_channelDestroyed()));
    return s;
}

QSshTcpSocket * QSshClient::openTcpSocket(const QString & hostName,quint16 port){
    if(sshClientPrivate->d_state != QSshClient::STATE_ACTIVATE_CHANNEL){
        qSshDebug("cannot open channel before connected()");
        return NULL;
    }
    QSshTcpSocket * s=new QSshTcpSocket(this);
    sshClientPrivate->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),sshClientPrivate,SLOT(d_channelDestroyed()));
    s->d->openTcpSocket(hostName,port);
    return s;
}



QSshClientPrivate::QSshClientPrivate()
    :d_session(0)
    ,d_state(QSshClient::STATE_NOT_CONNECTED)
    ,d_errorCode(0)
{
    d_session = ssh_new();
    d_state = QSshClient::STATE_NOT_CONNECTED;
    d_errorCode = 0;
    d_errorMessage = QString();
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


void QSshClientPrivate::d_readyRead(){

    if(d_state == QSshClient::STATE_TCP_CONNECTED){
        int ret=0;

        //1) initalise ssh session. exchange banner and stuff.
        if(ssh_options_set(d_session, SSH_OPTIONS_HOST, d_hostName.toLatin1()) < 0) {
            return;
        }

        if(ssh_options_set(d_session, SSH_OPTIONS_USER, d_userName.toLatin1()) < 0) {
            return;
        }

        if((ret = ssh_connect(d_session)) == SSH_AGAIN){
            return;
        }

        if (ret) {
            d_state = QSshClient::STATE_NOT_CONNECTED;
            qSshDebug("Failure establishing SSH session: %d", ret);
            emit sshClient->error(QSshClient::UnexpectedShutdownError);
            d_reset();
            return;
        }

        //2) make sure remote is safe.
        int type = 0;
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

        switch (type){
            case SSH_KEYTYPE_RSA:
                d_hostKey.type = QSshKey::Rsa;
                break;
            case SSH_KEYTYPE_RSA1:
                d_hostKey.type = QSshKey::Rsa1;
                break;
            case SSH_KEYTYPE_DSS:
                d_hostKey.type = QSshKey::Dss;
                break;
            case SSH_KEYTYPE_ECDSA:
                d_hostKey.type = QSshKey::Ecdsa;
                break;
            case SSH_KEYTYPE_ED25519:
                d_hostKey.type = QSshKey::Ed25519;
                break;
            default:
                d_hostKey.type = QSshKey::UnknownType;
        }

        if(fingerprint != "") {

            int state = ssh_is_server_known(d_session);
// for test
state = SSH_SERVER_KNOWN_OK;
            switch(state){
                case SSH_SERVER_KNOWN_OK:
                    d_state = QSshClient::STATE_SERVER_KNOWN;
                    d_readyRead();
                    return;
                case SSH_SERVER_KNOWN_CHANGED:
                    d_delaydError = QSshClient::ServerKnownChanged;
                    break;
                case SSH_SERVER_FOUND_OTHER:
                    d_delaydError = QSshClient::ServerFoundOther;
                    break;
                case SSH_SERVER_NOT_KNOWN:
                    d_delaydError = QSshClient::ServerNotKnown;
                    break;
                case SSH_SERVER_FILE_NOT_FOUND:
                    d_delaydError = QSshClient::ServerFileNotFound;
                    break;
                case SSH_SERVER_ERROR:
                    d_delaydError = QSshClient::ServerError;
                    break;
            }
        } else {
            d_delaydError=QSshClient::HostKeyInvalidError;
        }

        d_getLastError();
        d_reset();        
        QTimer::singleShot(0, this, SLOT(d_delaydErrorEmit()));
        return;

    } else if(d_state == QSshClient::STATE_SERVER_KNOWN){
        //3) try auth type "none" and get a list of other methods
        //   in the likely case that the server doesnt like "none"
        int rc = ssh_userauth_none(d_session, NULL);

        switch(rc) {
            case SSH_AUTH_ERROR:
                qSshDebug() << "SSH_AUTH_ERROR";
                break;
            case SSH_AUTH_DENIED:
                qSshDebug() << "SSH_AUTH_DENIED";
                break;
            case SSH_AUTH_PARTIAL:
                qSshDebug() << "SSH_AUTH_DENIED";
                break;
            case SSH_AUTH_SUCCESS:
                qSshDebug() << "SSH_AUTH_SUCCESS";
                break;
            case SSH_AUTH_AGAIN:
                qSshDebug() << "SSH_AUTH_AGAIN";
                break;
        }

        int authList = ssh_userauth_list(d_session, NULL);

        if(authList & SSH_AUTH_METHOD_PASSWORD) {
            qSshDebug() << "SSH_AUTH_METHOD_PASSWORD allowed";
            d_availableMethods<<QSshClient::PasswordAuthentication;
        }
        if(authList & SSH_AUTH_METHOD_PUBLICKEY) {
            qSshDebug() << "SSH_AUTH_METHOD_PUBLICKEY allowed";
            d_availableMethods<<QSshClient::PublicKeyAuthentication;

        }
        if(authList & SSH_AUTH_METHOD_HOSTBASED) {
            qSshDebug() << "SSH_AUTH_METHOD_HOSTBASED allowed";
            d_availableMethods<<QSshClient::HostBasedAuthentication;
        }
        if(authList & SSH_AUTH_METHOD_INTERACTIVE) {
            qSshDebug() << "SSH_AUTH_METHOD_INTERACTIVE allowed";
            d_availableMethods<<QSshClient::InteractiveAuthentication;
        }

        if(authList == 0){
            if(rc == SSH_AUTH_SUCCESS){
                //null auth ok
                emit sshClient->connected();
                d_state = QSshClient::STATE_TRY_TO_AUTHENTICATE;
                return;
            }else if(rc==SSH_AUTH_AGAIN) {
                return;
            }else{
                d_getLastError();
                emit sshClient->error(QSshClient::UnexpectedShutdownError);
                d_reset();
                emit sshClient->disconnected();
                return;
            }
        }

        d_state = QSshClient::STATE_AUTHENTICATE_MODE;
        d_readyRead();
    } else if(d_state == QSshClient::STATE_AUTHENTICATE_MODE) {
        if(d_availableMethods.contains(QSshClient::PublicKeyAuthentication) &&
                !d_privateKey.isNull() &&
                !d_failedMethods.contains(QSshClient::PublicKeyAuthentication)){

            qSshDebug() << "auth by key";
            d_currentAuthTry=QSshClient::PublicKeyAuthentication;
            d_state = QSshClient::STATE_TRY_TO_AUTHENTICATE;
            d_readyRead();
            return;
        }
        if(d_availableMethods.contains(QSshClient::PasswordAuthentication) &&
                !d_passphrase.isNull() &&
                !d_failedMethods.contains(QSshClient::PasswordAuthentication)){
            qSshDebug() << "auth by password";
            d_currentAuthTry=QSshClient::PasswordAuthentication;
            d_state = QSshClient::STATE_TRY_TO_AUTHENTICATE;
            d_readyRead();
            return;
        }

        qSshDebug() << "auth required";

        emit sshClient->authenticationRequired(d_availableMethods);

    } else if(d_state == QSshClient::STATE_TRY_TO_AUTHENTICATE) {
        int ret(0);
        if(d_currentAuthTry == QSshClient::PasswordAuthentication){
            ret = ssh_userauth_password(d_session, qPrintable(d_userName),
                                        qPrintable(d_passphrase));
        } else if(d_currentAuthTry == QSshClient::PublicKeyAuthentication) {
            ret = ssh_userauth_publickey_auto(d_session,
                                              qPrintable(d_userName),
                                              qPrintable(d_passphrase));
        }

        if(ret == SSH_AGAIN ) {
            return;
        } else if(ret == 0) {
            d_state = QSshClient::STATE_ACTIVATE_CHANNEL;
            qSshDebug() << "Connected";
            emit sshClient->connected();
        } else {
            d_getLastError();
            emit sshClient->error(QSshClient::AuthenticationError);
            d_failedMethods.append(d_currentAuthTry);
            d_state = QSshClient::STATE_AUTHENTICATE_MODE;
            d_readyRead();
        }
    } else if(d_state == QSshClient::STATE_ACTIVATE_CHANNEL) {
        QList<QSshChannel*>::const_iterator i;
        for (i = d_channels.constBegin(); i != d_channels.constEnd(); ++i) {
            bool ret=(*i)->d->activate();
            if(!ret) {
                d_getLastError();
            }
        }
    }else{
        qSshDebug("did not expect to receive data in this state");
    }
}

void QSshClientPrivate::d_reset(){

    if(d_state > QSshClient::STATE_TCP_CONNECTED){
        ssh_disconnect(d_session);
    }

    if(d_session){
        ssh_free(d_session);
    }

    d_state = QSshClient::STATE_NOT_CONNECTED;
    d_errorCode = 0;
    d_errorMessage = QString();
    d_failedMethods.clear();
    d_availableMethods.clear();
}

void QSshClientPrivate::d_disconnected (){
    if(d_state != QSshClient::STATE_NOT_CONNECTED){
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
    emit sshClient->error(d_delaydError);
}
