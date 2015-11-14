#include "qsshclient.h"
#include "qsshclientprivate.h"
#include "qsshchannel.h"

class QSshChannelPrivate : public QObject{
    Q_OBJECT
public:
    QSshChannelPrivate(QSshChannel*,QSshClient *);
    QSshChannel * p;
    QSshClient * d_client;
    ssh_channel  d_channel;
    ssh_session  d_session;
    int d_read_stream_id;
    int d_write_stream_id;

    int d_state;
    bool activate();

    QList<int> d_next_actions;
    QString d_cmd;
    QByteArray d_pty;
    void openSession();
    void requestPty(QByteArray pty);
    void start(QString cmd);
    void startShell();

    void openTcpSocket(QString host,qint16 port);
    QString d_host;
    qint16 d_port;
};

