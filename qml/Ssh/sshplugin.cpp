#include <qqml.h>
#include "sshplugin.h"
#include "qsshterminal.h"

using namespace Konsole;

void SshPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QSshTerminal>(uri, 1, 0, "Ssh");
    qmlRegisterType<TerminalDisplay>(uri, 1, 0, "QMLTermWidget");
    qmlRegisterType<KSession>(uri, 1, 0, "QMLTermSession");

}
