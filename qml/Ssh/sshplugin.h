#ifndef SSHPLUGIN_H
#define SSHPLUGIN_H

#include <QQmlExtensionPlugin>

class SshPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.majuca.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // SSHPLUGIN_H
