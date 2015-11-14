#ifndef QSSHCHANNEL_H
#define QSSHCHANNEL_H

#include <QIODevice>
#include "qssh_global.h"

class QSshClient;
class QSshChannelPrivate;

class QSSHSHARED_EXPORT QSshChannel : public QIODevice
{
     Q_OBJECT
public:
    virtual ~QSshChannel();

protected:
    QSshChannel(QSshClient*);
    virtual qint64 readData(char*, qint64);
    virtual qint64 writeData(const char*, qint64);
    virtual bool isSequential () const;

    QSshChannelPrivate * d;
    friend class QSshChannelPrivate;
    friend class QSshClientPrivate;
signals:
    void connected();
};

#endif // QSSHCHANNEL_H
