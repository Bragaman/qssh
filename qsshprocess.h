#ifndef QSSHPROCESS_H
#define QSSHPROCESS_H

#include "qsshchannel.h"
#include "qssh_global.h"

class QSSHSHARED_EXPORT QSshProcess : public QSshChannel {
    Q_OBJECT
 public:
    enum TerminalType{
        VanillaTerminal,
        Vt102Terminal,
        AnsiTerminal
    };
    void requestPty(TerminalType term=VanillaTerminal);
    void startShell();
    void start(const QString & cmd);
 signals:
    void started();
    void finished ( int exitCode);
 private:
    QSshProcess(QSshClient*);
    friend class QSshClient;
};

#endif // QSSHPROCESS_H
