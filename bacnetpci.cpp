#include "bacnetpci.h"

#include "bitfields.h"

BacnetConfirmedRequestData::BacnetConfirmedRequestData()
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
    _maxSegments = (((*ptr)>>4) & 0x0f);
    _maxResponses = (*ptr) & 0x0f;

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

    _serviceChoice = (BacnetConfirmedService::BacnetConfirmedServiceChoice)(*ptr);
    ++ptr;

    return (ptr - dataPtr);
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

qint16 BacnetPciData::fillRawResponse(quint8 *buffer)
{
    Q_UNUSED(buffer);
    return 0;
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
