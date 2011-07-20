#ifndef BACNETBBMDHANDLER_H
#define BACNETBBMDHANDLER_H

#include <QVector>
#include <QtGlobal>
#include <QHostAddress>

#include "bacnetvirtuallinklayer.h"
#include "bacnetbipaddress.h"

struct BbmdTableEntry
{
    //here I concretize BACnet address to BIP, since no other one is used in case of BBMD
    BacnetBipAddress address;
    BacnetBipMask mask;
};

class BacnetUdpTransportLayerHandler;
class BacnetBbmdHandler
{
public:
    BacnetBbmdHandler(BacnetUdpTransportLayerHandler *transportHndlr);

    //! Udpdates/initializes new BDT table. If succeeds, return Successcufll Completion code, otherwise NAK.
    bool setBroadcastTableFromRaw(quint8 *data, quint16 length);

    //! Writes the broadcast table contents into the array starting at data pointer.
    bool setBroatcastTableToRaw(quint8 *data, quint16 maxLength);

    //! Returns the size of the actual BDT array
    quint16 expectedBroatcastTableRawSize();

    /**
      Checks if the message (of type Forwarded-NPDU) is to be broadcasted locally or not.
      If positive, does so using bvlHnldlr functions.
      */
    void processForwardedMessage(quint8 *data, quint16 length);

private:
    QVector<BbmdTableEntry> _bbmdTable;
    BacnetUdpTransportLayerHandler *_transportHndlr;
};

#endif // BACNETBBMDHANDLER_H
