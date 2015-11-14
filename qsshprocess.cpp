#include "qsshprocess.h"
#include "qsshchannelprivate.h"

QSshProcess::QSshProcess(QSshClient * parent)
    :QSshChannel(parent)
{
    d->openSession();
    connect(this,SIGNAL(connected()),this,SIGNAL(started()));
}
/*!
 * Starts a login shell on the SSH server.
 *
 * If there is already a process running on this channel, the behavior is undefined.
 */
void QSshProcess::startShell(){
    d->startShell();
}
/*!
 * Invokes a shell command on the SSH server.
 *
 * If there is already a process running on this channel, the behavior is undefined.
 */
void QSshProcess::start(const QString & cmd){
    d->start(cmd);
}
/*!
 * Requests that a PTY be allocated for this channel on the remote host.
 *
 * This function must be invoked before starting the process that requires it.
 */
void QSshProcess::requestPty(TerminalType term){
    switch (term){
        case VanillaTerminal:
            d->requestPty("vanilla");
            break;;
        case Vt102Terminal:
            d->requestPty("vt102");
            break;;
        case AnsiTerminal:
            d->requestPty("ansi");
            break;;
    }
}

