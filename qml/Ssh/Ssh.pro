TEMPLATE = lib

TARGET = ssh

QT += qml quick widgets

CONFIG += qt plugin
CONFIG += release


TARGET = $$qtLibraryTarget($$TARGET)

uri = Ssh

VERSION = 1.0.0
QMAKE_TARGET_PRODUCT = Qml ssh plugin
QMAKE_TARGET_COPYRIGHT = copyright 2015 Jean-Luc Gyger

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

HEADERS += \
    sshplugin.h \
    BlockArray.h \
    mman.h \
    Character.h \
    CharacterColor.h \
    ColorScheme.h \
    tools.h \
    DefaultTranslatorText.h \
    ExtendedDefaultTranslator.h \
    Filter.h \
    TerminalCharacterDecoder.h \
    konsole_wcwidth.h \
    TerminalDisplay.h \
    ksession.h \
    Session.h \
    History.h \
    ScreenWindow.h \
    LineFont.h \
    KeyboardTranslator.h \
    HistorySearch.h \
    Emulation.h \
    kptyprocess.h \
    kprocess.h \
    ShellCommand.h \
    Vt102Emulation.h \
    Screen.h \
    extendedchartable.h

SOURCES += \
    sshplugin.cpp \
    BlockArray.cpp \
    mman.c \
    ColorScheme.cpp \
    tools.cpp \
    Filter.cpp \
    TerminalCharacterDecoder.cpp \
    konsole_wcwidth.cpp \
    TerminalDisplay.cpp \
    ksession.cpp \
    Session.cpp \
    History.cpp \
    ScreenWindow.cpp \
    KeyboardTranslator.cpp \
    HistorySearch.cpp \
    Emulation.cpp \
    kptyprocess.cpp \
    kprocess.cpp \
    ShellCommand.cpp \
    Vt102Emulation.cpp \
    Screen.cpp \
    extendedchartable.cpp

OTHER_FILES = Ssh/qmldir

DISTFILES += \
    Ssh/SshTerminal.qml \
    Ssh/QMLTermScrollbar.qml

DESTDIR = ./Ssh

INCLUDEPATH += "../../src"
LIBS += -L"../../src/release" -lqssh


DISTFILES += \
    default.keytab \
    LineFont.src
