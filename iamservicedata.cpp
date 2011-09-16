#include "iamservicedata.h"

#include "bacnetcoder.h"
#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

IAmServiceData::IAmServiceData(ObjectIdStruct &objId, quint32 maxAPDULength, BacnetSegmentation segmentation, quint32 vendorId):
        _objId(objId),
        _maxApduLength(maxAPDULength),
        _segmentationSupported(segmentation),
        _vendorId(vendorId)
{
}

IAmServiceData::IAmServiceData():
        _maxApduLength(Bacnet::ApduMaxSize),
        _segmentationSupported(SegmentedNOT),
        _vendorId(SNGVendorIdentifier)
{
    _objId.objectType = BacnetObjectType::Undefined;
}

qint32 IAmServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    qint32 ret(0);
    quint8 *actualPtr(startPtr);

    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, _objId, false, AppTags::BacnetObjectIdentifier);
    if (ret < 0)
        return -1;
    actualPtr += ret;
    buffLength -= ret;

    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _maxApduLength, false, AppTags::UnsignedInteger);
    if (ret < 0)
        return -2;
    actualPtr += ret;
    buffLength -= ret;

    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _segmentationSupported, false, AppTags::Enumerated);
    if (ret < 0)
        return -3;
    actualPtr += ret;
    buffLength -= ret;

    ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _vendorId, false, AppTags::UnsignedInteger);
    if (ret < 0)
        return -4;
    actualPtr += ret;

    return (actualPtr - startPtr);
}

qint32 IAmServiceData::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    Q_CHECK_PTR(serviceData);
    BacnetTagParser bParser(serviceData, buffLength);

    qint16 ret;
    qint16 consumedBytes(0);
    bool convOkOrCtxt;

    //parse object identifier
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::BacnetObjectIdentifier))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _objId = bParser.toObjectId(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //parse max APDU length accepted
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::UnsignedInteger))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _maxApduLength = bParser.toUInt(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //parse segmentation supported
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::Enumerated))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _segmentationSupported = (Bacnet::BacnetSegmentation)bParser.toEumerated(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //vendor Identifier
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::UnsignedInteger))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _vendorId = bParser.toUInt(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    if (bParser.hasNext())
        return -BacnetReject::ReasonTooManyArguments;

    return consumedBytes;
}
