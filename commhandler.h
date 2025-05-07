#ifndef COMMHANDLER_H
#define COMMHANDLER_H

#include <QObject>

#include <QMutex>
#include <QDataStream>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QTimer>

#include "wfviewtypes.h"

// This class abstracts the comm port in a useful way and connects to
// the command creator and command parser.

class commHandler : public QObject
{
    Q_OBJECT

public:
    commHandler(QObject* parent = nullptr);
    commHandler(QString portName, quint32 baudRate, quint8 wfFormat,QObject* parent = nullptr);
    bool serialError;
    bool rtsStatus();

    ~commHandler();

public slots:
    void setPTTType(pttType_t ptt);
    void handleError(QSerialPort::SerialPortError error);
    void init();

private slots:
    void receiveDataIn(); // from physical port
    void receiveDataFromUserToRig(const QByteArray &data);
    void debugThis();
    void setHalfDuplex(bool en);
    void sendDataOut(const QByteArray &writeData); // out to radio

signals:
    void haveTextMessage(QString message); // status, debug only
    void sendDataOutToPort(const QByteArray &writeData); // used to re-send data
    void haveDataFromPort(QByteArray data); // emit this when we have data, connect to rigcommander
    void havePortError(errorType err);
    void haveStatusUpdate(const QString text);

private:
    void setupComm();
    void openPort();
    void closePort();


    void debugMe();
    void hexPrint();

    //QDataStream stream;
    QByteArray outPortData;
    QByteArray inPortData;
    QByteArray previousSent;
    bool halfDuplex;
    QMutex duplexMutex;

    //QDataStream outStream;
    //QDataStream inStream;

    quint8 buffer[256];

    QString portName;
    QSerialPort *port=Q_NULLPTR;
    qint32 baudrate;
    quint8 stopbits;
    bool rolledBack;

    QSerialPort *pseudoterm;
    int ptfd; // pseudo-terminal file desc.
    mutable QMutex ptMutex;
    bool havePt;
    QString ptDevSlave;

    pttType_t pttType = pttCIV;

    bool isConnected; // port opened
    mutable QMutex mutex;
    void printHex(const QByteArray &pdata, bool printVert, bool printHoriz);
    bool combineWf = false;
    QByteArray spectrumData;
    quint8 spectrumDivisionNumber;
    quint8 spectrumDivisionMax;
    quint8 spectrumCenterOrFixed;
    quint8 spectrumInformation;
    quint8 lastSpectrum = 0;
    QTime lastDataReceived = QTime::currentTime();
};

#endif // COMMHANDLER_H
