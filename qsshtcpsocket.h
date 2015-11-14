#ifndef QSSHTCPSOCKET_H
#define QSSHTCPSOCKET_H

#include "qssh_global.h"
#include "qsshchannel.h"

class QSSHSHARED_EXPORT QSshTcpSocket : public QSshChannel
{
    Q_OBJECT
    private:
       QSshTcpSocket(QSshClient*);
       friend class QSshClient;
};

#endif // QSSHTCPSOCKET_H
