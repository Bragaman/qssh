#include <QCoreApplication>
#include "qemulator.h"
#include "qssh_global.h"

QEmulator::QEmulator(QObject *parent) : QObject(parent)
{
    esc = false;
    rawData = "";
}

void QEmulator::parseCommand(QByteArray cmd)
{
    rawData += cmd;
qSshDebug() << rawData;
    QByteArray parsedData = "";
    while(rawData.count() && !esc) {
        if(rawData.at(0) == QEmulator::ACC_ESC) {
            esc = true;
            rawData.remove(0,1);
            rawData = controlSequence();
        } else {
            if(!esc) {
                if(rawData.at(0) == QEmulator::ACC_CR ) {
                    rawData.remove(0,1);
                    continue;
                }
                parsedData += rawData.at(0);
                rawData.remove(0,1);
            } else {
                rawData = controlSequence();
            }
        }
    }



    emit printableChar(QString::fromUtf8(parsedData));
}

QByteArray QEmulator::controlSequence()
{
    // be sure to have enough char and to have
    if(rawData.count() >= 2) {
        // title hack
        if(
            rawData.count() > 4 &&
            rawData.at(0) == ']' &&
            rawData.at(1) == '0' &&
            rawData.at(2) == ';' &&
            rawData.contains(QEmulator::ACC_BEL)
          ){
            int index = 3;
            QString title = "";
            QChar c = rawData.at(index);
            if(c != QEmulator::ACC_BEL) {
                title += c;
            } else {
                index++;
            }
            while(c != QEmulator::ACC_BEL) {
                index++;
                c = rawData.at(index);
                if(c != QEmulator::ACC_BEL) {
                    title += c;
                } else {
                    index++;
                    esc = false;
                    emit terminalTitle(title);
                }
            }
            // remove sequence
            rawData.remove(0,index);

        }

        if(
            rawData.at(0) == '[' &&
            rawData.contains('m')
          ){
            int index = 1;
            QString colorSequence = "";
            QChar c = rawData.at(index);
            if(c != 'm') {
                colorSequence += c;
            } else {
                index++;
            }
            while(c != 'm') {
                index++;
                c = rawData.at(index);
                if(c != 'm') {
                    colorSequence += c;
                } else {
                    index++;
                    esc = false;
                }
            }
            // remove sequence
            rawData.remove(0,index);

        }
    }

    esc = false;


    return rawData;
}
