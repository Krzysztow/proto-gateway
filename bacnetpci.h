#ifndef BACNETPCI_H
#define BACNETPCI_H

#include <QtCore>

#include "bitfields.h"
#include "bacnetcommon.h"

class BacnetPciData;
class BacnetPci
{
public:
    enum ParseReturnCodes {
        BufferTooSmall = -1,
        InappropriateBufferSize = -2,
        UnexpectedType = -3
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
    static BacnetPciData *createPciData(quint8 *pciPtr, quint16 length, qint16 *retCode);
};

class BacnetPciData
{
public:
    virtual qint16 toRaw(quint8 *buffer, quint16 length) = 0;
    virtual quint8 pduType() = 0;
};

class BacnetConfirmedRequestData:
        public BacnetPciData
{
public:
    enum MaxSegmentsAccepted {
        Segments_Unspecified     = 0x000,
        Segments_TwoSegments     = 0x001,
        Segments_FourSemgnets    = 0x010,
        Segments_EightSegments   = 0x011,
        Segments_SixteenSegs     = 0x100,
        Segments_ThirtyTwoSegs   = 0x101,
        Segments_SixtyFourSegs   = 0x110,
        Segments_More64Segs      = 0x111
    };

    enum MaxLengthAccepted {
        Length_UptToMinimumMessageSize  = 0x00,
        Length_128Octets                = 0x01,
        Length_206Octets                = 0x02,
        Length_480Octets                = 0x03,
        Length_1024Octets               = 0x04,
        Length_1476Octets               = 0x05,
        Length_MaxBacnetReserver        = 0x08
    };

public:
    BacnetConfirmedRequestData();
    BacnetConfirmedRequestData(bool segmented, bool moreFollows, bool segmentedAccepted, MaxSegmentsAccepted maxSegments, MaxLengthAccepted maxLengthAccepted,
                               quint8 invokeId, BacnetServicesNS::BacnetConfirmedServiceChoice service, quint8 sequenceNumber = 0, quint8 windowSize = 0);
    BacnetConfirmedRequestData(MaxLengthAccepted maxLengthAccepted, quint8 invokeId, BacnetServicesNS::BacnetConfirmedServiceChoice service);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);

    inline BacnetServicesNS::BacnetConfirmedServiceChoice service() {return _serviceChoice;}
    inline quint8 invokedId() {return _invokeId;}

    inline bool isSegmented() {return _segmented;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    bool _segmented;
    bool _moreFollows;
    bool _segmentedRespAccepted;
    MaxSegmentsAccepted _maxSegments;
    MaxLengthAccepted _maxResponses;
    quint8 _invokeId;
    quint8 _sequenceNum;
    quint8 _propWindowSize;
    BacnetServicesNS::BacnetConfirmedServiceChoice _serviceChoice;

};

class BacnetUnconfirmedRequestData:
        public BacnetPciData
{
public:
    BacnetUnconfirmedRequestData() {}
    BacnetUnconfirmedRequestData(quint8 serviceChoice);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline BacnetServicesNS::BacnetUnconfirmedServiceChoice service() {return (BacnetServicesNS::BacnetUnconfirmedServiceChoice)_serviceChoice;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    quint8 _serviceChoice;
};

class BacnetSimpleAckData:
        public BacnetPciData
{
public:
    BacnetSimpleAckData() {}
    BacnetSimpleAckData(quint8 invokeId, quint8 serviceChoice);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline quint8 invokeId() {return _invokeId;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    quint8 _invokeId;
    quint8 _serviceAckChoice;
};

class BacnetComplexAckData:
        public BacnetPciData
{
public:
    BacnetComplexAckData() {}
    BacnetComplexAckData(quint8 invokeId, quint8 serviceAckChoice,
                         quint8 sequenceNumber = 0, quint8 propWindSize = 0,
                         bool segmented = false, bool moreFollows = false);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline quint8 invokeId() {return _origInvokeId;}
    inline bool isSegmented() {return _segmented;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    bool _segmented;
    bool _moreFollows;
    quint8 _origInvokeId;
    quint8 _seqNum;
    quint8 _propWindSize;
    quint8 _serviceAckChoice;
};

class BacnetSegmentedAckData:
        public BacnetPciData
{
public:
    BacnetSegmentedAckData() {}
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline quint8 invokeId() {return _origInvokeId;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

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

class BacnetErrorData:
        public BacnetPciData
{
public:
    BacnetErrorData() {}
    BacnetErrorData(quint8 origInvokeId, quint8 errorChoice);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline quint8 invokeId() {return _origInvokeId;}

    quint8 errorChoice();

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    quint8 _origInvokeId;
    quint8 _errorChoice;
};

class BacnetRejectData:
        public BacnetPciData
{
public:
    BacnetRejectData() {}
    BacnetRejectData(quint8 originalInvokeId, quint8 rejectReason);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline quint8 invokeId() {return _origInvokeId;}

    inline quint8 rejectReason() {return _rejectReason;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    quint8 _origInvokeId;
    quint8 _rejectReason;
};

class BacnetAbortData:
        public BacnetPciData
{
public:
    BacnetAbortData() {}
    BacnetAbortData(quint8 originalInvokeId, quint8 abortReason, bool fromServer);
    qint16 fromRaw(quint8 *dataPtr, quint16 length);
    inline quint8 invokeId() {return _origInvokeId;}

public://overridden from BacnetPciData
    virtual quint8 pduType();
    virtual qint16 toRaw(quint8 *buffer, quint16 length);

private:
    bool _sentByServer;
    quint8 _origInvokeId;
    quint8 _abortReason;
};

#endif // BACNETPCI_H
