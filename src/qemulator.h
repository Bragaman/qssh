#ifndef QEMULATOR_H
#define QEMULATOR_H

#include <QObject>

class QEmulator : public QObject
{
    Q_OBJECT

    // ACC control system
    enum ACC {
        ACC_NULL,   //Null char
        ACC_SOH,    //Start of Heading
        ACC_STX,    //Start of Text
        ACC_ETX,    //End of Text
        ACC_EOT,    //End of Transmission
        ACC_ENQ,    //Enquiry
        ACC_ACK,    //Acknowledgment
        ACC_BEL,    //Bell
        ACC_BS,     //Back Space
        ACC_HT,     //Horizontal Tab
        ACC_LF,     //Line Feed
        ACC_VT,     //Vertical Tab
        ACC_FF,     //Form Feed
        ACC_CR,     //Carriage Return
        ACC_SO,     //Shift Out / X-On
        ACC_SI,     //Shift In / X-Off
        ACC_DEL,    //Data Line Escape
        ACC_DC1,    //Device Control 1 (oft. XON)
        ACC_DC2,    //Device Control 2
        ACC_DC3,    //Device Control 3 (oft. XOFF)
        ACC_DC4,    //Device Control 4
        ACC_NAK,    //Negative Acknowledgement
        ACC_SYN,    //Synchronous Idle
        ACC_ETB,    //End of Transmit Block
        ACC_CAN,    //Cancel
        ACC_EM,     //End of Medium
        ACC_SUB,    //Substitute
        ACC_ESC,    //Escape
        ACC_FS,     //File Separator
        ACC_GS,     //Group Separator
        ACC_RS,     //Record Separator
        ACC_US      //Unit Separator
    };

    // ASCII printable char
    enum APC {
        APC_CB = 93 // Closing bracket
    };

public:
    explicit QEmulator(QObject *parent = 0);

signals:
    void printableChar(QString c);
    void terminalTitle(QString t);

public slots:
    virtual void parseCommand(QString cmd);

protected:
    virtual QString controlSequence();


protected:
    QString rawData;
    bool esc;
};

#endif // QEMULATOR_H
