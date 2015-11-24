#ifndef QSSHTERMINAL_H
#define QSSHTERMINAL_H

#include <QObject>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QPalette>

#include "qsshclient.h"
#include "TerminalDisplay.h"

using namespace Konsole;

class QSshTerminal : public TerminalDisplay
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

    Q_PROPERTY(int ptyCols READ ptyCols WRITE setPtyCols
               NOTIFY ptyColsChanged FINAL)

    Q_PROPERTY(int ptyRows READ ptyRows WRITE setPtyRows
               NOTIFY ptyRowsChanged FINAL)

public:
    explicit QSshTerminal(QQuickItem *parent = 0);

    Q_INVOKABLE void connectToHost();
    Q_INVOKABLE void writeOnShell(QString text);
    Q_INVOKABLE int getFontWidth(int pixelSize);

    QString host() const;
    int port() const;
    QString username() const;
    QString passphrase() const;
    int ptyCols() const;
    int ptyRows() const;

signals:
    void hostChanged(QString value);
    void portChanged(int value);
    void usernameChanged(QString value);
    void passphraseChanged(QString value);
    void ptyColsChanged(int value);
    void ptyRowsChanged(int value);

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
    void setPtyCols(int value);
    void setPtyRows(int value);

protected slots:
    void hostConnected();

private:
    QSshClient *m_sshClient;
    QSshProcess *m_sshProcess;

    QString m_host;
    int m_port;
    QString m_username;
    QString m_passphrase;
    int m_ptyCols;
    int m_ptyRows;

};


#endif // QSSHTERMINAL_H
