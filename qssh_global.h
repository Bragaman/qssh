#ifndef QSSH_GLOBAL_H
#define QSSH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QSSH_LIBRARY)
#  define QSSHSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QSSHSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QSSH_GLOBAL_H
