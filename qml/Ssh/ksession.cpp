/*
    This file is part of Konsole QML plugin,
    which is a terminal emulator from KDE.

    Copyright 2013      by Dmitry Zagnoyko <hiroshidi@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/

// Own
#include "ksession.h"

// Qt
#include <QTextCodec>

// Konsole
#include "KeyboardTranslator.h"
#include "HistorySearch.h"

KSession::KSession(QObject *parent) :
    QObject(parent), m_session(createSession(""))
{
    connect(m_session, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_session, SIGNAL(finished()), this, SLOT(sessionFinished()));
    connect(m_session, SIGNAL(titleChanged()), this, SIGNAL(titleChanged()));
    connect(m_session, SIGNAL(error(int,QString)), this, SIGNAL(error(int,QString)));
}

KSession::~KSession()
{
    if (m_session) {
        m_session->close();
        m_session->disconnect();
        delete m_session;
    }
}

void KSession::setTitle(QString name)
{
    m_session->setTitle(Session::NameRole, name);
}


Session *KSession::createSession(QString name)
{
    Session *session = new Session();

    session->setTitle(Session::NameRole, name);

    /* Thats a freaking bad idea!!!!
     * /bin/bash is not there on every system
     * better set it to the current $SHELL
     * Maybe you can also make a list available and then let the widget-owner decide what to use.
     * By setting it to $SHELL right away we actually make the first filecheck obsolete.
     * But as iam not sure if you want to do anything else ill just let both checks in and set this to $SHELL anyway.
     */

    //cool-old-term: There is another check in the code. Not sure if useful.
    session->setAutoClose(true);

    session->setCodec(QTextCodec::codecForName("UTF-8"));

    session->setHistoryType(HistoryTypeBuffer(1000));

    session->setDarkBackground(true);

    session->setKeyBindings("");

    return session;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


int  KSession::getRandomSeed()
{
    return m_session->sessionId() * 31;
}

void  KSession::addView(TerminalDisplay *display)
{
    m_session->addView(display);
}

void KSession::removeView(TerminalDisplay *display)
{
    m_session->removeView(display);
}

void KSession::sessionFinished()
{
    emit finished();
}

void KSession::selectionChanged(bool textSelected)
{
    Q_UNUSED(textSelected)
}

void KSession::acceptUnkownHost() {
    m_session->acceptUnkownHost();
}

void KSession::startShellProgram()
{
    if ( m_session->isRunning() ) {
        qDebug() << "Session already running";
        return;
    }

    qDebug() << "Start the session";
    m_session->run();
}

void KSession::setEnvironment(const QStringList &environment)
{
    m_session->setEnvironment(environment);
}

void KSession::setInitialWorkingDirectory(const QString &dir)
{
    if ( _initialWorkingDirectory == dir ) {
        _initialWorkingDirectory = dir;
        m_session->setInitialWorkingDirectory(dir);
        emit initialWorkingDirectoryChanged();
}   }

QString KSession::getInitialWorkingDirectory()
{
    return _initialWorkingDirectory;
}

void KSession::setTextCodec(QTextCodec *codec)
{
    m_session->setCodec(codec);
}

void KSession::setHistorySize(int lines)
{
    if ( historySize() != lines ) {
        if (lines < 0)
            m_session->setHistoryType(HistoryTypeFile());
        else
            m_session->setHistoryType(HistoryTypeBuffer(lines));
        emit historySizeChanged();
    }
}

int KSession::historySize() const
{
    if ( m_session->historyType().isUnlimited() ) {
        return -1;
    } else {
        return m_session->historyType().maximumLineCount();
    }
}

void KSession::sendText(QString text)
{
    m_session->sendText(text);
}

void KSession::sendKey(int rep, int key, int mod) const
{
    //TODO implement or remove this function.
//    Qt::KeyboardModifier kbm = Qt::KeyboardModifier(mod);

//    QKeyEvent qkey(QEvent::KeyPress, key, kbm);

//    while (rep > 0){
//        m_session->sendKey(&qkey);
//        --rep;
    //    }
}

void KSession::search(const QString &regexp, int startLine, int startColumn, bool forwards)
{
    HistorySearch *history = new HistorySearch( QPointer<Emulation>(m_session->emulation()), QRegExp(regexp), forwards, startColumn, startLine, this);
    connect( history, SIGNAL(matchFound(int,int,int,int)), this, SIGNAL(matchFound(int,int,int,int)));
    connect( history, SIGNAL(noMatchFound()), this, SIGNAL(noMatchFound()));
    history->search();
}

void KSession::setKeyBindings(const QString &kb)
{
    m_session->setKeyBindings(kb);
    emit changedKeyBindings(kb);
}

QString KSession::getKeyBindings()
{
   return m_session->keyBindings();
}

QStringList KSession::availableKeyBindings()
{
    return KeyboardTranslatorManager::instance()->allTranslators();
}

QString KSession::keyBindings()
{
    return m_session->keyBindings();
}

QString KSession::getTitle()
{
    return m_session->userTitle();
}

QString KSession::host() const
{
    return m_session->host();
}

int KSession::port() const
{
    return m_session->port();
}

QString KSession::username() const
{
    return  m_session->username();
}

QString KSession::passphrase() const
{
    return m_session->passphrase();
}

void KSession::setHost(QString value)
{
    if(value != m_session->host()) {
        m_session->setHost(value);
        emit hostChanged(value);
    }
}

void KSession::setPort(int value)
{
    if(value != m_session->port()) {
        m_session->setPort(value);
        emit portChanged(value);
    }

}

void KSession::setUsername(QString value)
{
    if(value != m_session->username()) {
        m_session->setUsername(value);
        emit usernameChanged(value);
    }

}

void KSession::setPassphrase(QString value)
{
    if(value != m_session->passphrase()) {
        m_session->setPassphrase(value);
        emit hostChanged(value);
    }
}
