#ifndef QSSHTERMINAL_H
#define QSSHTERMINAL_H

#include <QObject>
#include "qsshclient.h"

class QSshTerminal : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host READ host WRITE setHost
               NOTIFY hostChanged FINAL)

    Q_PROPERTY(int port READ port WRITE setPort
               NOTIFY portChanged FINAL)

    Q_PROPERTY(QString username READ username WRITE setUsername
               NOTIFY usernameChanged FINAL)

    Q_PROPERTY(QString passphrase READ passphrase WRITE setPassphrase
               NOTIFY passphraseChanged FINAL)


public:
    explicit QSshTerminal(QObject *parent = 0);

    Q_INVOKABLE void connectToHost();
    Q_INVOKABLE void writeOnShell(QString text);

    QString host() const;
    int port() const;
    QString username() const;
    QString passphrase() const;

signals:
    void hostChanged(QString value);
    void portChanged(int value);
    void usernameChanged(QString value);
    void passphraseChanged(QString value);

    void connected();
    void error(int error, QString message);
    void cmdResponse(QString response);
    void endCmdResponse();
    void shellRead(QString data);

public slots:
    void setHost(QString value);
    void setPort(int value);
    void setUsername(QString value);
    void setPassphrase(QString value);

protected slots:
    void hostConnected();

protected:
    QSshClient *m_sshClient;
    QSshProcess *m_sshProcess;

    QString m_host;
    int m_port;
    QString m_username;
    QString m_passphrase;
};

#endif // QSSHTERMINAL_H
