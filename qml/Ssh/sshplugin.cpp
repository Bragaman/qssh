#include <qqml.h>
#include "sshplugin.h"
#include "qsshterminal.h"


void SshPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QSshTerminal>(uri, 1, 0, "SshTerminal");
}
