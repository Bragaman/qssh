#include <QCoreApplication>
#include "qemulator.h"
#include "qssh_global.h"

QEmulator::QEmulator(QObject *parent) : QObject(parent)
{
    esc = false;
    rawData = "";
}

void QEmulator::parseCommand(QString cmd)
{
    rawData += cmd;

    while(rawData.count() && !esc) {
        if(rawData.at(0).toLatin1() == QEmulator::ACC_ESC) {
            esc = true;
            rawData.remove(0,1);
            rawData = controlSequence();
        } else {
            if(!esc) {
                emit printableChar(rawData.at(0));
                rawData.remove(0,1);
                QCoreApplication::processEvents();
            } else {
                rawData = controlSequence();
            }
        }
    }
}

QString QEmulator::controlSequence()
{
    // be sure to have enough char and to have
    if(rawData.count() >= 2) {
        // title hack
        if(
            rawData.count() > 4 &&
            rawData.at(0).toLatin1() == ']' &&
            rawData.at(1).toLatin1() == '0' &&
            rawData.at(2).toLatin1() == ';' &&
            rawData.contains(QEmulator::ACC_BEL)
          ){
            qSshDebug() << "title hack";
            int index = 3;
            QChar c = rawData.at(index).toLatin1();
            QString title;
            if(c != QEmulator::ACC_BEL) {
                title += c;
            }
            while(c != QEmulator::ACC_BEL) {
                index++;
                c = rawData.at(index).toLatin1();
                if(c != QEmulator::ACC_BEL) {
                    title += c;
                } else {
                    esc = false;
                    qSshDebug() << "title hack found" << title ;
                    emit terminalTitle(title);
                }
            }
            // remove sequence
            rawData.remove(0,index);

        }
    }

    esc = false;

    qSshDebug() << rawData;
    return rawData;
}

