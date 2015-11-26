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

#ifndef KSESSION_H
#define KSESSION_H

#include <QObject>

// Konsole
#include "Session.h"

using namespace Konsole;

class KSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString  kbScheme  READ  getKeyBindings WRITE setKeyBindings NOTIFY changedKeyBindings)
    Q_PROPERTY(QString  initialWorkingDirectory READ getInitialWorkingDirectory WRITE setInitialWorkingDirectory NOTIFY initialWorkingDirectoryChanged)
    Q_PROPERTY(QString  title READ getTitle WRITE setTitle NOTIFY titleChanged)

    // SSH connection properties
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged FINAL)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged FINAL)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged FINAL)
    Q_PROPERTY(QString passphrase READ passphrase WRITE setPassphrase NOTIFY passphraseChanged FINAL)


public:
    KSession(QObject *parent = 0);
    ~KSession();

    QString host() const;
    int port() const;
    QString username() const;
    QString passphrase() const;

    void addView(TerminalDisplay *display);
    void removeView(TerminalDisplay *display);

    int getRandomSeed();
    QString getKeyBindings();

    //look-n-feel, if you don`t like defaults

    //environment
    void setEnvironment(const QStringList & environment);

    //Initial working directory
    void setInitialWorkingDirectory(const QString & dir);    
    QString getInitialWorkingDirectory();

    //Text codec, default is UTF-8
    void setTextCodec(QTextCodec * codec);

    // History size for scrolling
    void setHistorySize(int lines); //infinite if lines < 0
    int historySize() const;

    /*! Get all available keyboard bindings
     */
    static QStringList availableKeyBindings();

    //! Return current key bindings
    QString keyBindings();

    QString getTitle();

signals:
    void hostChanged(QString value);
    void portChanged(int value);
    void usernameChanged(QString value);
    void passphraseChanged(QString value);


    void started();
    void finished();
    void copyAvailable(bool);

    void termGetFocus();
    void termLostFocus();

    void termKeyPressed(QKeyEvent *);

    void changedKeyBindings(QString kb);

    void titleChanged();

    void historySizeChanged();

    void initialWorkingDirectoryChanged();

    void matchFound(int startColumn, int startLine, int endColumn, int endLine);
    void noMatchFound();

public slots:

    /**
     * Set your host ssh server address
     * @brief hostChanged
     * @param value
     */
    void setHost(QString value);

    /**
     * Set your host ssh port
     * @brief hostChanged
     * @param value
     */
    void setPort(int value);

    /**
     * Set the userame used for the ssh authentication
     * @brief usernameChanged
     * @param value
     */
    void setUsername(QString value);

    /**
     * Set the password when authentication is done by username/password
     * or the passphrase of the certificate
     * @brief passphraseChanged
     * @param value
     */
    void setPassphrase(QString value);

    /*! Set named key binding for given widget
     */
    void setKeyBindings(const QString & kb);

    void setTitle(QString name);

    /**
     * Start the shell programm by openning the SSH connection
     * @brief startShellProgram
     */
    void startShellProgram();

    // Send some text to terminal
    void sendText(QString text);

    // Send some text to terminal
    void sendKey(int rep, int key, int mod) const;

    // Search history
    void search(const QString &regexp, int startLine = 0, int startColumn = 0, bool forwards = true );

protected slots:
    void sessionFinished();
    void selectionChanged(bool textSelected);

private slots:
    Session* createSession(QString name);

private:
    QString _initialWorkingDirectory;
    Session *m_session;
};

#endif // KSESSION_H
