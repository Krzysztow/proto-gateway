#ifndef BACNETPCI_H
#define BACNETPCI_H

#include <QtCore>

#include "bitfields.h"
#include "bacnetcommon.h"

class BacnetPci
{
public:
    enum ParseReturnCodes {
        BufferTooSmall = -1,
        InappropriateBufferSize = -2
    };

    enum BacnetPduType
    {
        TypeConfirmedRequest    = 0x00,
        TypeUnconfirmedRequest  = 0x01,
        TypeSimpleAck           = 0x02,
        TypeComplexAck          = 0x03,
        TypeSemgmendAck         = 0x04,
        TypeError               = 0x05,
        TypeReject              = 0x06,
        TypeAbort               = 0x07
    };

    inline static BacnetPduType pduType(quint8 *pciPtr) {return (BacnetPduType)((*pciPtr)>>4);}
};

class BacnetConfirmedRequestData
{
public:
    BacnetConfirmedRequestData();
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

    inline BacnetConfirmedService::BacnetConfirmedServiceChoice service() {return _serviceChoice;}

private:
    bool _segmented;
    bool _moreFollows;
    bool _segmentedRespAccepted;
    quint8 _maxSegments;
    quint8 _maxResponses;
    quint8 _invokeId;
    quint8 _sequenceNum;
    quint8 _propWindowSize;
    BacnetConfirmedService::BacnetConfirmedServiceChoice _serviceChoice;

};

class BacnetUnconfirmedRequestData
{
public:
    BacnetUnconfirmedRequestData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

private:
    quint8 _serviceChoice;
};

class BacnetSimpleAckData
{
public:
    BacnetSimpleAckData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

private:
    quint8 _invokeId;
    quint8 _serviceAckChoice;
};

class BacnetComplexAckData
{
public:
    BacnetComplexAckData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

private:
    bool _segmented;
    bool _moreFollows;
    quint8 _origInvokeId;
    quint8 _seqNum;
    quint8 _propWindSize;
    quint8 _serviceAckChoice;
};

class BacnetSegmentedAckData
{
public:
    BacnetSegmentedAckData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

    enum Acknowledgment {
        SegmentOk = 0x00,
        SegmentOutOfOrder = BitFields::Bit1
    };

private:
    Acknowledgment _negativeAck;
    bool _sentByServer;
    quint8 _origInvokeId;
    quint8 _seqNum;
    quint8 _actualWindSize;
};

class BacnetErrorData
{
public:
    BacnetErrorData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

private:
    quint8 _origInvokeId;
    quint8 _errorChoice;
};

class BacnetRejectData
{
public:
    BacnetRejectData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

private:
    quint8 _origInvokeId;
    quint8 _rejectReason;
};

class BacnetAbortData
{
public:
    BacnetAbortData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

private:
    bool _sentByServer;
    quint8 _origInvokeId;
    quint8 _abortReason;
};

#endif // BACNETPCI_H
