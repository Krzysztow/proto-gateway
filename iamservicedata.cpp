#include "iamservicedata.h"

#include "bacnetcoder.h"
#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

IAmServiceData::IAmServiceData(ObjectIdentifier &objId, quint32 maxAPDULength, BacnetSegmentation segmentation, quint32 vendorId):
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
}

qint32 IAmServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    qint32 ret(0);
    quint8 *actualPtr(startPtr);

    ret = _objId.toRaw(actualPtr, buffLength);
    if (ret < 0)
        return ret;
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
    ret = _objId.fromRaw(bParser);
    if (ret < 0)
        return -BacnetRejectNS::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //parse max APDU length accepted
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::UnsignedInteger))
        return -BacnetRejectNS::ReasonMissingRequiredParameter;
    _maxApduLength = bParser.toUInt(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetRejectNS::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //parse segmentation supported
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::Enumerated))
        return -BacnetRejectNS::ReasonMissingRequiredParameter;
    _segmentationSupported = (Bacnet::BacnetSegmentation)bParser.toEumerated(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetRejectNS::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //vendor Identifier
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::UnsignedInteger))
        return -BacnetRejectNS::ReasonMissingRequiredParameter;
    _vendorId = bParser.toUInt(&convOkOrCtxt);
    if (!convOkOrCtxt)
        return -BacnetRejectNS::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    if (bParser.hasNext())
        return -BacnetRejectNS::ReasonTooManyArguments;

    return consumedBytes;
}
