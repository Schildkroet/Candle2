#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H


#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTcpSocket>
#include <QString>


typedef enum
{
    IF_SERIAL = 0, IF_ETHERNET
} Interface_e;


void SerialIf_Init();
void SerialIf_DeInit();

bool SerialIf_OpenSerial(int protocol, QString portname, qint32 baud);
bool SerialIf_OpenEth(QString ip, qint32 port);

void SerialIf_Close();
bool SerialIf_IsOpen();

void SerialIf_Clear();

QString SerialIf_GetError();

qint64 SerialIf_Write(const QByteArray &data);
qint64 SerialIf_Write(const char *data, qint64 len);

bool SerialIf_CanReadLine();
qint64 SerialIf_IsDataAvailable();

QByteArray SerialIf_ReadLine();
qint64 SerialIf_Read(quint8 *data, qint64 max);


/*class SerialInterface : public QObject
{
public:
    SerialInterface();
    ~SerialInterface();

    bool OpenSerial(int protocol, QString portname, qint32 baud);
    bool OpenEth(QString ip, qint32 port);

    virtual void Close();
    virtual bool IsOpen();

    virtual qint64 Write(const QByteArray &data);
    virtual qint64 Write(const char *data, qint64 len);

    virtual bool CanReadLine();
    virtual QByteArray ReadLine();

private:
    QSerialPort m_serialPort;
    QTcpSocket m_tcpSocket;
    Interface_e m_Interface;
    int m_Protocol;
    bool m_SocketOpen;

};*/


#endif // SERIALINTERFACE_H
