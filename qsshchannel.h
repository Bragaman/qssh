#ifndef QSSHCHANNEL_H
#define QSSHCHANNEL_H

#include <QIODevice>
#include "qssh_global.h"

class QSshClient;
class QSshChannelPrivate;

class QSSHSHARED_EXPORT QSshChannel : public QObject
{
     Q_OBJECT
public:
    virtual ~QSshChannel();

protected:
    QSshChannel(QSshClient*);
    QSshChannelPrivate * d;
    friend class QSshChannelPrivate;
    friend class QSshClientPrivate;
signals:
    void connected();
};

#endif // QSSHCHANNEL_H
