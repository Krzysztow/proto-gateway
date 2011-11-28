#include "bacnetpci.h"

#include "bitfields.h"

BacnetConfirmedRequestData::BacnetConfirmedRequestData()
{
}

BacnetConfirmedRequestData::BacnetConfirmedRequestData(bool segmented, bool moreFollows, bool segmentedAccepted, MaxSegmentsAccepted maxSegments, MaxLengthAccepted maxLengthAccepted,
                                                       quint8 invokeId, BacnetServicesNS::BacnetConfirmedServiceChoice service, quint8 sequenceNumber, quint8 windowSize):
_segmented(segmented),
_moreFollows(moreFollows),
_segmentedRespAccepted(segmentedAccepted),
_maxSegments(maxSegments),
_maxResponses(maxLengthAccepted),
_invokeId(invokeId),
_sequenceNum(sequenceNumber),
_propWindowSize(windowSize),
_serviceChoice(service)
{

}

BacnetConfirmedRequestData::BacnetConfirmedRequestData(MaxLengthAccepted maxLengthAccepted, quint8 invokeId, BacnetServicesNS::BacnetConfirmedServiceChoice service):
        _segmented(false),
        _moreFollows(false),
        _segmentedRespAccepted(false),
        _maxSegments(Segments_Unspecified),
        _maxResponses(maxLengthAccepted),
        _invokeId(invokeId),
        _sequenceNum(0),
        _propWindowSize(0),
        _serviceChoice(service)
{

}


qint16 BacnetConfirmedRequestData::fromRaw(quint8 *dataPtr, quint16 length)
{
    quint8 *ptr = dataPtr;

    //get infromatio from first byte
    _segmented = ((*ptr) & BitFields::Bit3);
    _moreFollows = ((*ptr) & BitFields::Bit2);
    _segmentedRespAccepted = ((*ptr) & BitFields::Bit1);

    Q_ASSERT(length >= (4+2*_segmented));
    if (length < (4+2*_segmented))
        return BacnetPci::BufferTooSmall;
    //parse further
    ++ptr;
    _maxSegments = (MaxSegmentsAccepted)(((*ptr)>>4) & 0x0f);
    _maxResponses = (MaxLengthAccepted)((*ptr) & 0x0f);

    ++ptr;
    _invokeId = *ptr;

    ++ptr;
    if (_segmented) {
        _sequenceNum = *ptr;
        ++ptr;
        _propWindowSize = *ptr;
        ++ptr;
    } else {
        _sequenceNum = _propWindowSize = 0;
    }

    _serviceChoice = (BacnetServicesNS::BacnetConfirmedServiceChoice)(*ptr);
    ++ptr;

    return (ptr - dataPtr);
}

qint16 BacnetConfirmedRequestData::toRaw(quint8 *buffer, quint16 length)
{
    if (_segmented) {
        if (length < 5) {
            Q_ASSERT(false);
            return -1;//can't write to buffer
        }
    } else {
        if (length < 3) {
            Q_ASSERT(false);
            return -1;
        }
    }

    quint8 *actualPtr(buffer);
    *actualPtr = (0x0f & BacnetPci::TypeConfirmedRequest) << 4;
    if (_segmented)
        *actualPtr |= BitFields::Bit3;
    if (_moreFollows)
        *actualPtr |= BitFields::Bit2;
    if (_segmentedRespAccepted)
        *actualPtr |= BitFields::Bit1;
    ++actualPtr;
    Q_ASSERT(_maxSegments <= 0x07);
    Q_ASSERT(_maxResponses <= 0x0f);
    *actualPtr = ( (_maxSegments & 0x07) << 4) | ( (_maxResponses & 0xf) );
    ++actualPtr;
    *actualPtr = _invokeId;
    ++actualPtr;
    if (_segmented) {
        *actualPtr = _sequenceNum;
        ++actualPtr;
        *actualPtr = _propWindowSize;
        ++actualPtr;
    }
    *actualPtr = _serviceChoice;
    ++actualPtr;
    return (actualPtr - buffer);
}

qint16 BacnetUnconfirmedRequestData::fromRaw(quint8 *dataPtr, quint16 length)
{
    Q_ASSERT(length >= 1);
    if (length < 1)
        return BacnetPci::BufferTooSmall;

    quint8 *ptr = dataPtr;
    ++ptr;
    _serviceChoice = *ptr;
    ++ptr;

    return (ptr - dataPtr);
}

BacnetUnconfirmedRequestData::BacnetUnconfirmedRequestData(quint8 serviceChoice):
        _serviceChoice(serviceChoice)
{
}

quint8 BacnetUnconfirmedRequestData::pduType()
{
    return BacnetPci::TypeUnconfirmedRequest;
}

qint16 BacnetUnconfirmedRequestData::toRaw(quint8 *buffer, quint16 length)
{
    Q_ASSERT(length >= 2);
    if (length <= 2)
        return BacnetPci::BufferTooSmall;

    *buffer = (BacnetPci::TypeUnconfirmedRequest & 0x0f) << 4;
    ++buffer;
    *buffer = _serviceChoice;

    return 2;
}

BacnetSimpleAckData::BacnetSimpleAckData(quint8 invokeId, quint8 serviceChoice):
        _invokeId(invokeId),
        _serviceAckChoice(serviceChoice)
{
}

qint16 BacnetSimpleAckData::fromRaw(quint8 *dataPtr, quint16 length)
{
    Q_ASSERT(length >= 3);
    if (length < 3)
        return BacnetPci::BufferTooSmall;

    quint8 *ptr = dataPtr;
    ++ptr;
    _invokeId = *ptr;
    ++ptr;
    _serviceAckChoice = *ptr;
    ++ptr;

    return (ptr - dataPtr);
}

quint8 BacnetSimpleAckData::pduType()
{
    return BacnetPci::TypeSimpleAck;
}

qint16 BacnetSimpleAckData::toRaw(quint8 *buffer, quint16 length)
{
    if (length < 3) {
        Q_ASSERT(false);
        return -1;
    }

    quint8 *actualPtr(buffer);
    *actualPtr = ((0x0f & BacnetPci::TypeSimpleAck) << 4);
    ++actualPtr;
    *actualPtr = _invokeId;
    ++actualPtr;
    *actualPtr = _serviceAckChoice;
    ++actualPtr;
    return (actualPtr - buffer);
}

BacnetComplexAckData::BacnetComplexAckData(quint8 invokeId, quint8 serviceAckChoice,
                                           quint8 sequenceNumber, quint8 propWindSize,
                                           bool segmented, bool moreFollows):
_segmented(segmented),
_moreFollows(moreFollows),
_origInvokeId(invokeId),
_seqNum(sequenceNumber),
_propWindSize(propWindSize),
_serviceAckChoice(serviceAckChoice)
{
    Q_ASSERT(segmented ? (_propWindSize > 0) : true);
}

qint16 BacnetComplexAckData::fromRaw(quint8 *dataPtr, quint16 length)
{
    quint8 *ptr = dataPtr;
    _segmented = ((*ptr) & BitFields::Bit3);
    _moreFollows = ((*ptr) & BitFields::Bit2);

    Q_ASSERT( length >= (5 + 2*_segmented));
    if (length < (5 + 2*_segmented))
        return BacnetPci::BufferTooSmall;

    ++ptr;
    _origInvokeId = *ptr;
    ++ptr;
    if (_segmented) {
        _seqNum = *ptr;
        ++ptr;
        _propWindSize = *ptr;
        ++ptr;
    } else {
        _seqNum = _propWindSize = 0;
    }
    _serviceAckChoice = *ptr;
    ++ptr;

    return (ptr - dataPtr);
}

quint8 BacnetComplexAckData::pduType()
{
    return BacnetPci::TypeComplexAck;
}

qint16 BacnetComplexAckData::toRaw(quint8 *buffer, quint16 length)
{
    if (_segmented) {
        Q_ASSERT(length >= 5);
        if (length < 5)
            return -1;
    } else {
        Q_ASSERT(length >= 3);
        if (length < 3)
            return -1;
    }

    quint8* actualPtr(buffer);
    *actualPtr = ( (0x0f & BacnetPci::TypeComplexAck) << 4 );
    if (_segmented)
        *actualPtr |= BitFields::Bit3;
    if (_moreFollows)
        *actualPtr |= BitFields::Bit2;
    ++actualPtr;
    *actualPtr = _origInvokeId;
    ++actualPtr;
    if (_segmented) {
        *actualPtr = _seqNum;
        ++actualPtr;
        *actualPtr = _propWindSize;
        ++actualPtr;
    }
    *actualPtr = _serviceAckChoice;
    ++actualPtr;
    return (actualPtr - buffer);
}

qint16 BacnetSegmentedAckData::fromRaw(quint8 *dataPtr, quint16 length)
{
    Q_ASSERT(length == 4);
    if (length != 4)
        return BacnetPci::InappropriateBufferSize;

    quint8 *ptr = dataPtr;
    _negativeAck = (Acknowledgment)((*ptr) & BitFields::Bit1);
    _sentByServer = ((*ptr) & BitFields::Bit0);

    ++ptr;
    _origInvokeId = *ptr;
    ++ptr;
    _seqNum = *ptr;
    ++ptr;
    _actualWindSize = *ptr;

    return (ptr - dataPtr);
}

quint8 BacnetSegmentedAckData::pduType()
{
    return BacnetPci::TypeSemgmendAck;
}

qint16 BacnetSegmentedAckData::toRaw(quint8 *buffer, quint16 length)
{
    Q_ASSERT(length == 4);
    if (length != 4)
        return BacnetPci::InappropriateBufferSize;

    _negativeAck = (Acknowledgment)(*buffer & BitFields::Bit1);
    _sentByServer = *buffer & BitFields::Bit0;
    ++buffer;
    _origInvokeId = *buffer;
    ++buffer;
    _seqNum = *buffer;
    ++buffer;
    _actualWindSize = *buffer;

    return 4;
}

qint16 BacnetErrorData::fromRaw(quint8 *dataPtr, quint16 length)
{
    Q_ASSERT(length >= 3);
    if (length < 3)
        return BacnetPci::BufferTooSmall;

    quint8 *ptr = dataPtr;
    ++ptr;
    _origInvokeId = *ptr;
    ++ptr;
    _errorChoice = *ptr;

    return (ptr - dataPtr);
}

BacnetRejectData::BacnetRejectData(quint8 originalInvokeId, quint8 rejectReason):
        _origInvokeId(originalInvokeId),
        _rejectReason(rejectReason)
{
}

qint16 BacnetRejectData::fromRaw(quint8 *dataPtr, quint16 length)
{
    Q_ASSERT(length >= 3);
    if (length < 3)
        return BacnetPci::BufferTooSmall;

    quint8 *ptr = dataPtr;
    ++ptr;
    _origInvokeId = *ptr;
    ++ptr;
    _rejectReason = *ptr;

    return (ptr - dataPtr);
}

qint16 BacnetRejectData::toRaw(quint8 *buffer, quint16 length)
{
    Q_ASSERT(length >= 3);
    if (length < 3)
        return BacnetPci::BufferTooSmall;

    *buffer = (BacnetPci::TypeReject & 0x0F) << 4;
    ++buffer;
    *buffer = _origInvokeId;
    ++buffer;
    *buffer = _rejectReason;
    return 3;
}

quint8 BacnetRejectData::pduType()
{
    return BacnetPci::TypeReject;
}

BacnetAbortData::BacnetAbortData(quint8 originalInvokeId, quint8 abortReason, bool fromServer):
    _sentByServer(fromServer),
    _origInvokeId(originalInvokeId),
    _abortReason(abortReason)
{
}

qint16 BacnetAbortData::fromRaw(quint8 *dataPtr, quint16 length)
{
    Q_ASSERT(length >= 3);
    if (length < 3)
        return BacnetPci::BufferTooSmall;

    quint8 *ptr = dataPtr;
    _sentByServer = ((*ptr) & BitFields::Bit0);
    ++ptr;
    _origInvokeId = *ptr;
    ++ptr;
    _abortReason = *ptr;

    return (ptr - dataPtr);
}

quint8 BacnetAbortData::pduType()
{
    return BacnetPci::TypeAbort;
}

qint16 BacnetAbortData::toRaw(quint8 *buffer, quint16 length)
{
    Q_ASSERT(length >= 3);
    if (length < 3)
        return BacnetPci::BufferTooSmall;

    _sentByServer = *buffer & BitFields::Bit0;
    ++buffer;
    _origInvokeId = *buffer;
    ++buffer;
    _abortReason = *buffer;

    return 3;
}

qint16 BacnetPciData::toRaw(quint8 *buffer, quint16 length)
{
    Q_UNUSED(buffer);
    Q_UNUSED(length);
    return -1;
}

BacnetPciData *BacnetPci::createPciData(quint8 *pciPtr, quint16 length, qint16 *retCode)
{
    Q_CHECK_PTR(retCode);

    switch (BacnetPci::pduType(pciPtr))
    {
    case (BacnetPci::TypeConfirmedRequest):
        {
            /*upon reception:
          - when no semgenation - do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
          - when segmented - respond with BacnetSegmentAck PDU and when all gotten, do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
         */
            BacnetConfirmedRequestData *bData = new BacnetConfirmedRequestData();
            *retCode = bData->fromRaw(pciPtr, length);
            return bData;
        }
    case (BacnetPci::TypeUnconfirmedRequest):
        /*upon reception: do what's needed and that's all
     */
        Q_ASSERT_X(false, "BacnetPci", "Factory doesn't implements this type, yet");
        break;
    case (BacnetPci::TypeSimpleAck):
        /*upon reception update state machine
     */
        Q_ASSERT_X(false, "BacnetPci", "Factory doesn't implements this type, yet");
        break;
    case (BacnetPci::TypeComplexAck):
        /*upon reception update state machine
     */
        Q_ASSERT_X(false, "BacnetPci", "Factory doesn't implements this type, yet");
        break;
    case (BacnetPci::TypeSemgmendAck):
        /*upon reception update state machine and send back another segment
     */
        Q_ASSERT_X(false, "BacnetPci", "Factory doesn't implements this type, yet");
        break;
    case (BacnetPci::TypeError):
        /*BacnetConfirmedRequest seervice failed
     */
        Q_ASSERT_X(false, "BacnetPci", "Factory doesn't implements this type, yet");
        break;
    case (BacnetPci::TypeReject):
        /*Protocol error occured
     */
        Q_ASSERT_X(false, "BacnetPci", "Factory doesn't implements this type, yet");
        break;
    case (BacnetPci::TypeAbort):
        break;
    default: {
            Q_ASSERT(false);
            *retCode = UnexpectedType;
            return 0;
        }
    }

    return 0;
}

quint8 BacnetConfirmedRequestData::pduType()
{
    return BacnetPci::TypeConfirmedRequest;
}


BacnetErrorData::BacnetErrorData(quint8 origInvokeId, quint8 errorChoice):
        _origInvokeId(origInvokeId),
        _errorChoice(errorChoice)
{
}

quint8 BacnetErrorData::pduType()
{
    return BacnetPci::TypeError;
}

qint16 BacnetErrorData::toRaw(quint8 *dataPtr, quint16 length)
{
    if (length < 3) {
        Q_ASSERT(false);
        return -1;//can't fit in the buffer length
    }

    quint8 *actualPtr(dataPtr);
    *actualPtr = ((0x0f & BacnetPci::TypeError) << 4);
    ++actualPtr;
    *actualPtr = _origInvokeId;
    ++actualPtr;
    *actualPtr = _errorChoice;
    ++actualPtr;
    return (actualPtr - dataPtr);
}

quint8 BacnetErrorData::errorChoice()
{
    return _errorChoice;
}
