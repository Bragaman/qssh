#ifndef QSSHTERMINAL_H
#define QSSHTERMINAL_H

#include <QObject>

class QSshTerminal : public QObject
{
    Q_OBJECT
public:
    explicit QSshTerminal(QObject *parent = 0);

signals:

public slots:
};

#endif // QSSHTERMINAL_H
