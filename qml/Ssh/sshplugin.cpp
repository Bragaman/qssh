#include <qqml.h>
#include "sshplugin.h"
#include "TerminalDisplay.h"

using namespace Konsole;

void SshPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<TerminalDisplay>(uri, 1, 0, "QMLTermWidget");
    qmlRegisterType<KSession>(uri, 1, 0, "QMLTermSession");

}
