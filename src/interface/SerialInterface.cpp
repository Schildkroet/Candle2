#include "SerialInterface.h"


QSerialPort m_serialPort;
QTcpSocket m_tcpSocket;
Interface_e m_Interface;
int m_Protocol;
bool m_SocketOpen;

QString m_ip;
qint32 m_port;


void SerialIf_Init()
{
    // Setup serial port
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort.setStopBits(QSerialPort::OneStop);

    m_Protocol = 0;
    m_SocketOpen = false;
}

void SerialIf_DeInit()
{
    if(m_serialPort.isOpen())
    {
        m_serialPort.close();
    }

    if(m_tcpSocket.isOpen())
    {
        m_tcpSocket.close();
        m_SocketOpen = false;
    }
}

bool SerialIf_OpenSerial(int protocol, QString portname, qint32 baud)
{
    m_serialPort.setPortName(portname);
    m_serialPort.setBaudRate(baud);

    m_Interface = IF_SERIAL;
    m_Protocol = protocol;

    return m_serialPort.open(QIODevice::ReadWrite);
}

bool SerialIf_OpenEth(QString ip, qint32 port)
{
    m_Interface = IF_ETHERNET;

    if(m_tcpSocket.isOpen())
    {
        // Close if already open
        m_tcpSocket.close();
    }

    m_ip = ip;
    m_port = port;

    m_tcpSocket.connectToHost(ip, port);

    m_SocketOpen = m_tcpSocket.waitForConnected(3000);

    return m_SocketOpen;
}

void SerialIf_Close()
{
    if(m_serialPort.isOpen())
    {
        m_serialPort.close();
    }

    if(m_tcpSocket.isOpen())
    {
        m_tcpSocket.close();
        m_SocketOpen = false;
    }
}

bool SerialIf_IsOpen()
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.isOpen();
    }
    else if(m_Interface == IF_ETHERNET)
    {
        return m_SocketOpen;
    }

    return false;
}

void SerialIf_Clear()
{
    m_serialPort.clear();
}

bool SerialIf_SetDTR(bool state)
{
    return m_serialPort.setDataTerminalReady(state);
}

QString SerialIf_GetError()
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.errorString();
    }
    else if(m_Interface == IF_ETHERNET)
    {
        return m_tcpSocket.errorString();
    }

    return QString("");
}

qint64 SerialIf_Write(const QByteArray &data)
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.write(data);
    }
    else if(m_Interface == IF_ETHERNET)
    {
        /*if(m_tcpSocket.state() != QTcpSocket::ConnectedState)
        {
            qDebug() << "Disconnected. Trying to reopen";
            m_tcpSocket.reset();

            m_tcpSocket.connectToHost(m_ip, m_port);
            m_SocketOpen = m_tcpSocket.waitForConnected(3000);
        }*/

        qint64 bytes = m_tcpSocket.write(data);
        if(bytes < 0)
        {
            qDebug() << "Error: " << m_tcpSocket.errorString();
        }
        m_tcpSocket.waitForBytesWritten(4);

        return bytes;
    }
    qDebug() << "Unknown interface";

    return 0;
}

qint64 SerialIf_Write(const char *data, qint64 len)
{
    return SerialIf_Write(QByteArray(data, len));
}

bool SerialIf_CanReadLine()
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.canReadLine();
    }
    else if(m_Interface == IF_ETHERNET)
    {
        return m_tcpSocket.canReadLine();
    }

    // No valid interface
    return false;
}

qint64 SerialIf_IsDataAvailable()
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.bytesAvailable();
    }
    else if(m_Interface == IF_ETHERNET)
    {
        return m_tcpSocket.bytesAvailable();
    }

    // No valid interface
    return 0;
}

QByteArray SerialIf_ReadLine()
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.readLine();
    }
    else if(m_Interface == IF_ETHERNET)
    {
        return m_tcpSocket.readLine();
    }

    // Return empty array
    return QByteArray();
}

qint64 SerialIf_Read(quint8 *data, qint64 max)
{
    if(m_Interface == IF_SERIAL)
    {
        return m_serialPort.read((char*)data, max);
    }
    else if(m_Interface == IF_ETHERNET)
    {
        return m_tcpSocket.read((char*)data, max);
    }

    // Return empty array
    return 0;
}
