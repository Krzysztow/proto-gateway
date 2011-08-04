#ifndef BACNETUDPTRANSPORTLAYER_H
#define BACNETUDPTRANSPORTLAYER_H

#include <QObject>
#include <QUdpSocket>

#include "bacnetbipaddress.h"

/**
  This class is to be used to take care of data transport.
  \note This transport layer is very tidly coupled to BVLL layer
  */

class BacnetBvllHandler;
class Buffer;
class BacnetUdpTransportLayerHandler :
        public QObject//inhertiance from QObject due to readDatagrams() slot
{
    Q_OBJECT
public:
    explicit BacnetUdpTransportLayerHandler(QObject *parent = 0);

    /**
      This function has two uses:
      # makes the UDPSocket listen on the port port;
      # tells this handler that all the packets comming from ip port are those that we have sent -
        so they are not propagated to upper layers.
      */
    bool setAddress(QHostAddress ip, quint16 port);

    //! Sets the instance of BVLL layer, which will all the datagrams be passed to.
    void setBvlc(BacnetBvllHandler *bvllHndlr);

    //! Used by higher layers to send data over UDP
    bool send(quint8 *data, qint64 length, QHostAddress destAddr, qint16 destPort);

    //! Newer version of above.
    void sendBuffer(Buffer *buffer, QHostAddress &destAddr, qint16 destPort);

    /**
      Returns actual ip address of the device
      \sa setAddress()
      */
    QHostAddress address();
    /**
      Returns actual port of the device.
      \sa setAddress()
      */
    quint16 port();

signals:

private slots:
    /**
      This slot is used to read pending datagrams.
      \note when reading datagram, this function checks if the frame source is not itself (basing on \sa address() & \sa port()).
      If so, then the datagram is discarded and not passed to upper layers.
      */
    void readDatagrams();

private:
    QUdpSocket *_socket;
    QByteArray _datagram;

    BacnetBvllHandler *_bvllHndlr;

    QHostAddress _myAddress;
    quint16 _myPort;
};

#endif // BACNETUDPTRANSPORTLAYER_H
