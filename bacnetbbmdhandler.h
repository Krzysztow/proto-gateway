#ifndef BACNETBBMDHANDLER_H
#define BACNETBBMDHANDLER_H

#include <QVector>
#include <QtGlobal>
#include <QHostAddress>

#include "bacnetvirtuallinklayer.h"
#include "bacnetbipaddress.h"

//fix that there can be no more than 5 foreign devices
#define MAX_FD_NUM 5

struct BbmdTableEntry
{
    //here I concretize BACnet address to BIP, since no other one is used in case of BBMD
    BacnetAddress address;
    //be careful, this is stored in local host byte ored
    quint32 mask;
};

struct FdTableEntry
{
    //again concretized, no need for abstractness
    BacnetAddress address;
    quint16 timeToLiveLeft;
};

class BacnetBbmdHandler
{
public:
    BacnetBbmdHandler(BacnetBvllHandler *bvllHandler);

    //! Udpdates/initializes new BDT table. If succeeds, return Successcufll Completion code, otherwise NAK.
    bool setBroadcastTableFromRaw(quint8 *data, quint16 length);

    //! Writes the broadcast table contents into the array starting at data pointer.
    bool setBroatcastTableToRaw(quint8 *data, quint16 maxLength);

    //! Returns the size of the actual BDT array
    quint16 expectedBroatcastTableRawSize();

    /**
      Checks if the message (of type Forwarded-NPDU) is to be broadcasted locally or not.
      If positive, does so using bvlHnldlr functions.
      \todo forward to all FDs except source
      */
    void processForwardedMessage(quint8 *data, quint16 length, BacnetAddress &srcAddr);

    /**
      Carries out actions that should be done when BBMD gets DistributeBroadcastToNetwork message:
      # broadcasts message locally
      # sends to all BBMDs (excludes itself!)
      # forwards to all Foreign Devices (if one of them has srcAddr, it is omitted (no replications))
      \warning data is already formed Forward-NPDU frame
      \todo forwarding to Foreign Devices is not implemented
      */
    void processBroadcastToNetwork(quint8 *data, quint16 length, BacnetAddress &srcAddr);

    /**
      \warning data is already formed Forward-NPDU frame
      Carries out actions for reception of Original-Broadcast-NPDU:
      # sends data to all BBMDs (exluding itself, no need to)
      # sends data to all FD
      */
    void processOriginalBroadcast(quint8 *data, quint16 length, BacnetAddress &srcAddr);

    /**
      \note maybe it's better pass entire frame received, not only the data
      \warning data points to the registration field! which contains time-to-live parameter. The address itself corresponds to FD.
      */
//    void processRegisterFD(quint8 *data, quint16 length, BacnetBipAddress &srcAddr);


private:
    /**
      Helper function, that sends data of length length to all devices in the BDT table.
      If excludeMyself is true. this will not be sent to the host device (its parameters
      are taken from \sa _transportHndlr).
      */
    void sendToBbmds(quint8 *data, quint16 length, bool excludeMyself = true);

private:
    QVector<BbmdTableEntry> _bbmdTable;
    QVector<FdTableEntry> _fdTable;
    BacnetBvllHandler *_bvllHndlr;
};

#endif // BACNETBBMDHANDLER_H
