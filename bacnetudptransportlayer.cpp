#include "bacnetudptransportlayer.h"

#include "bacnetcommon.h"
#include "bacnetvirtuallinklayer.h"

BacnetUdpTransportLayerHandler::BacnetUdpTransportLayerHandler(QObject *parent) :
    QObject(parent),
    _socket(new QUdpSocket(this)),
    _bvllHndlr(0)
{
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    //make space for _datagrams to be received
    _datagram.resize(BacnetCommon::BvllMaxSize);
}

bool BacnetUdpTransportLayerHandler::setAddress(QHostAddress ip, quint16 port)
{
    _myAddress = ip;
    _myPort = port;
    //will listen to the packets directed to the device/broadcasted with port port
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
        length = _socket->readDatagram(_datagram.data(), BacnetCommon::BvllMaxSize, &srcAddr, &srcPort);
        if (length > 0) {
            if ( (_myAddress == srcAddr) && (srcPort == _myPort) ) {
                qDebug("BacnetUdpTransportLayerHandler:readDatagrams() : Discard message received from myself!");
                continue;
            }
            //pass it to the higher layer if any data read
            _bvllHndlr->consumeDatagram((quint8*)(_datagram.data()), length, srcAddr, srcPort);
        }
    }
}

bool BacnetUdpTransportLayerHandler::send(quint8 *data, qint64 length, QHostAddress destAddr, qint16 destPort)
{
    return (_socket->writeDatagram((char*)data, length, destAddr, destPort) == length);
}

QHostAddress BacnetUdpTransportLayerHandler::address()
{
    return _myAddress;
}

quint16 BacnetUdpTransportLayerHandler::port()
{
    return _myPort;
}
