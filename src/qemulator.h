#ifndef QEMULATOR_H
#define QEMULATOR_H

#include <QObject>

// Reset
#define Color_Off "[0m"       // Text Reset

// Regular Colors
#define SQ_Black "[0;30m"        // Black
#define SQ_Red "[0;31m"          // Red
#define SQ_Green "[0;32m"        // Green
#define SQ_Yellow "[0;33m"       // Yellow
#define SQ_Blue "[0;34m"         // Blue
#define SQ_Purple "[0;35m"       // Purple
#define SQ_Cyan "[0;36m"         // Cyan
#define SQ_White "[0;37m"        // White

// Bold
#define SQ_BBlack "[1;30m"       // Black
#define SQ_BRed "[1;31m"         // Red
#define SQ_BGreen "[1;32m"       // Green
#define SQ_BYellow "[1;33m"      // Yellow
#define SQ_BBlue "[1;34m"        // Blue
#define SQ_BPurple "[1;35m"      // Purple
#define SQ_BCyan "[1;36m"        // Cyan
#define SQ_BWhite "[1;37m"       // White

// Underline
#define SQ_UBlack "[4;30m"       // Black
#define SQ_URed "[4;31m"         // Red
#define SQ_UGreen "[4;32m"       // Green
#define SQ_UYellow "[4;33m"      // Yellow
#define SQ_UBlue "[4;34m"        // Blue
#define SQ_UPurple "[4;35m"      // Purple
#define SQ_UCyan "[4;36m"        // Cyan
#define SQ_UWhite "[4;37m"       // White

// Background
#define SQ_On_Black "[40m"       // Black
#define SQ_On_Red "[41m"         // Red
#define SQ_On_Green "[42m"       // Green
#define SQ_On_Yellow "[43m"      // Yellow
#define SQ_On_Blue "[44m"        // Blue
#define SQ_On_Purple "[45m"      // Purple
#define SQ_On_Cyan "[46m"        // Cyan
#define SQ_On_White "[47m"       // White

// High Intensity
#define SQ_IBlack "[0;90m"       // Black
#define SQ_IRed "[0;91m"         // Red
#define SQ_IGreen "[0;92m"       // Green
#define SQ_IYellow "[0;93m"      // Yellow
#define SQ_IBlue "[0;94m"        // Blue
#define SQ_IPurple "[0;95m"      // Purple
#define SQ_ICyan "[0;96m"        // Cyan
#define SQ_IWhite "[0;97m"       // White

// Bold High Intensity
#define BIBlack "[1;90m"      // Black
#define BIRed "[1;91m"        // Red
#define BIGreen "[1;92m"      // Green
#define BIYellow "[1;93m"     // Yellow
#define BIBlue "[1;94m"       // Blue
#define BIPurple "[1;95m"     // Purple
#define BICyan "[1;96m"       // Cyan
#define BIWhite "[1;97m"      // White

// High Intensity backgrounds
#define On_IBlack "[0;100m"   // Black
#define On_IRed "[0;101m"     // Red
#define On_IGreen "[0;102m"   // Green
#define On_IYellow "[0;103m"  // Yellow
#define On_IBlue "[0;104m"    // Blue
#define On_IPurple "[0;105m"  // Purple
#define On_ICyan "[0;106m"    // Cyan
#define On_IWhite "[0;107m"   // White


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
    virtual void parseCommand(QByteArray cmd);

protected:
    virtual QByteArray controlSequence();

protected:
    QByteArray rawData;
    bool esc;
};

#endif // QEMULATOR_H
