#include "bacnetudptransportlayer.h"

#include "bacnetcommon.h"
#include "bacnetvirtuallinklayer.h"
#include "buffer.h"

BacnetUdpTransportLayerHandler::BacnetUdpTransportLayerHandler(QObject *parent) :
    QObject(parent),
    _socket(new QUdpSocket(this)),
    _bvllHndlr(0)
{
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    //make space for _datagrams to be received
    _datagram.resize(Bacnet::BvllMaxSize);
}

bool BacnetUdpTransportLayerHandler::setAddress(QHostAddress ip, quint16 port)
{
    _myAddress = ip;
    _myPort = port;
    //will listen to the packets directed to the device/broadcasted with port port
    qDebug("%s : address set to %s:%d", __PRETTY_FUNCTION__, qPrintable(ip.toString()), port);
    return _socket->bind(port, QUdpSocket::ShareAddress);
}

void BacnetUdpTransportLayerHandler::setBvlc(BacnetBvllHandler *bvllHndlr)
{
    _bvllHndlr = bvllHndlr;
    Q_ASSERT(0 != _bvllHndlr);
}

void BacnetUdpTransportLayerHandler::readDatagrams()
{
    Q_ASSERT(0 != _bvllHndlr);
    QHostAddress srcAddr;
    quint16 srcPort;
    qint64 length;
    while (_socket->hasPendingDatagrams()) {
        length = _socket->readDatagram(_datagram.data(), Bacnet::BvllMaxSize, &srcAddr, &srcPort);
        if (length > 0) {
            if ( (_myAddress == srcAddr) && (srcPort == _myPort) ) {
                qDebug("BacnetUdpTransportLayerHandler:readDatagrams() : Discard message received from myself!");
                continue;
            }
            qDebug("Got message from %s, %d. My settings: %s, %d", qPrintable(srcAddr.toString()), srcPort,
                    qPrintable(_myAddress.toString()), _myPort);
            //pass it to the higher layer if any data read
            _bvllHndlr->consumeDatagram((quint8*)(_datagram.data()), length, srcAddr, srcPort);
        }
    }
}

bool BacnetUdpTransportLayerHandler::send(quint8 *data, qint64 length, QHostAddress destAddr, quint16 destPort)
{
    return (_socket->writeDatagram((char*)data, length, destAddr, destPort) == length);
}

void BacnetUdpTransportLayerHandler::sendBuffer(Buffer *buffer, QHostAddress &destAddr, quint16 destPort)
{
    qDebug()<<"Data is being sent to"<<destAddr<<destPort;
    Buffer::printArray(buffer->bodyPtr(), buffer->bodyLength(), "Sending data:");

    if (buffer->bodyLength() != _socket->writeDatagram((char*)buffer->bodyPtr(), buffer->bodyLength(), destAddr, destPort)) {
        qDebug("I didn't manage to send the data! Need to implement resending!");
    }
}

QHostAddress BacnetUdpTransportLayerHandler::address()
{
    return _myAddress;
}

quint16 BacnetUdpTransportLayerHandler::port()
{
    return _myPort;
}
