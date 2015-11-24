#include "qsshterminal.h"
#include "qsshprocess.h"
#include <QFontMetricsF>
#include <QPainter>

using namespace Konsole;

QSshTerminal::QSshTerminal(QQuickItem *parent) : TerminalDisplay(parent)
{    
    m_host = "";
    m_port = 22;
    m_username = "";
    m_passphrase = "";
    m_sshClient = new QSshClient;
    m_sshProcess = NULL;
    m_ptyCols = 80;
    m_ptyRows = 24;

    connect(m_sshClient, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_sshClient, SIGNAL(connected()), this, SLOT(hostConnected()));
    connect(m_sshClient, SIGNAL(error(int, QString)), this, SIGNAL(error(int, QString)));
    connect(m_sshClient, SIGNAL(channelCmdResponse(QString)), this, SIGNAL(cmdResponse(QString)));
    connect(m_sshClient, SIGNAL(channelShellResponse(QString)), this, SIGNAL(shellRead(QString)));
    connect(m_sshClient, SIGNAL(channelEndCmdResponse()), this, SIGNAL(endCmdResponse()));
}


void QSshTerminal::connectToHost()
{

    m_sshClient->setPassphrase(m_passphrase);
    m_sshClient->connectToHost(m_username, m_host, m_port);
}

void QSshTerminal::writeOnShell(QString text)
{
    m_sshProcess->writeOnShell(text);
}

int QSshTerminal::getFontWidth(int pixelSize)
{
    QFont font("Courier New", pixelSize);
    QFontMetricsF fm(font);
    return fm.boundingRect("A").width();
}

QString QSshTerminal::host() const
{
    return m_host;
}

int QSshTerminal::port() const
{
    return m_port;
}

QString QSshTerminal::username() const
{
    return  m_username;
}

QString QSshTerminal::passphrase() const
{
    return m_passphrase;
}

int QSshTerminal::ptyCols() const
{
    return m_ptyCols;
}

int QSshTerminal::ptyRows() const
{
    return m_ptyRows;
}


void QSshTerminal::setHost(QString value)
{
    if(value != m_host) {
        m_host = value;
        emit hostChanged(value);
    }
}

void QSshTerminal::setPort(int value)
{
    if(value != m_port) {
        m_port = value;
        emit portChanged(value);
    }

}

void QSshTerminal::setUsername(QString value)
{
    if(value != m_username) {
        m_username = value;
        emit usernameChanged(value);
    }

}

void QSshTerminal::setPassphrase(QString value)
{
    if(value != m_passphrase) {
        m_passphrase = value;
        emit hostChanged(value);
    }
}

void QSshTerminal::setPtyCols(int value)
{
    if(value != m_ptyCols) {
        m_ptyCols = value;
        emit ptyColsChanged(value);
        if(m_sshProcess) {
            m_sshProcess->changePtySize(m_ptyCols, m_ptyRows);
        }
    }
}

void QSshTerminal::setPtyRows(int value)
{
    if(value != m_ptyRows) {
        m_ptyRows = value;
        emit ptyRowsChanged(value);
        if(m_sshProcess) {
            m_sshProcess->changePtySize(m_ptyCols, m_ptyRows);
        }
    }
}


void QSshTerminal::hostConnected()
{
    m_sshProcess = m_sshClient->openProcessChannel();
    if(m_sshProcess) {
        m_sshProcess->startShell();
        m_sshProcess->changePtySize(m_ptyCols, m_ptyRows);
    }

}
