#-------------------------------------------------
#
# Project created by QtCreator 2015-11-13T20:57:16
#
#-------------------------------------------------

QT       += network
QT       -= gui

TARGET = qssh
TEMPLATE = lib

DEFINES += QSSH_LIBRARY

SOURCES += \
    qsshclient.cpp \
    qsshchannel.cpp \
    qsshtcpsocket.cpp \
    qsshprocess.cpp

HEADERS +=\
        qssh_global.h \
    qsshclient.h \
    qsshclientprivate.h \
    qsshchannelprivate.h \
    qsshchannel.h \
    qsshtcpsocket.h \
    qsshprocess.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../libssh/include
win32:LIBS += -L$$PWD/../libssh/lib -llibssh
