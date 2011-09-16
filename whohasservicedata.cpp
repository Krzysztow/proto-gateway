#include "whohasservicedata.h"

#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

#error "This class hasn't been implemented yet - ANYHOW!"

WhoHasServiceData::WhoHasServiceData():
        _rangeLowLimit(Bacnet::InvalidInstanceNumber),
        _rangeHighLimit(Bacnet::InvalidInstanceNumber),
        _objidentifier(0),
        _objName(0)
{
}

qint32 WhoHasServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
//    if ( (0 == _rangeLowLimit) ||
//         (Bacnet::MaximumInstanceNumber == _rangeHighLimit) )
//        return 0;//nothing to encode

//    Q_CHECK_PTR(startPtr);
//    quint8 *actualPtr(startPtr);
//    quint16 leftLength(buffLength);
//    qint32 ret;

//    //encode low range limit
//    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _rangeLowLimit, true, 0);
//    if (ret <= 0) {//something wrong?
//        Q_ASSERT_X(false, "WhoIsServiceData::toRaw()", "Cannot encode low range.");
//        qDebug("WhoIsServiceData::toRaw() : Cannot encode low range: %d", ret);
//        return ret;
//    }
//    actualPtr += ret;
//    leftLength -= ret;

//    //encode high range limit
//    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _rangeHighLimit, true, 1);
//    if (ret <= 0) {//something wrong?
//        Q_ASSERT_X(false, "WhoIsServiceData::toRaw()", "Cannot encode high range.");
//        qDebug("WhoIsServiceData::toRaw() : Cannot encode high range: %d", ret);
//        return ret;
//    }
//    actualPtr += ret;

//    return (actualPtr - startPtr);
}

qint32 WhoHasServiceData::fromRaw(quint8 *serviceData, quint16 buffLength)
{
//    Q_CHECK_PTR(serviceData);
//    BacnetTagParser bParser(serviceData, buffLength);

//    qint16 ret;
//    qint16 consumedBytes(0);
//    bool convOkOrCtxt;

//    //parse object identifier
//    ret = bParser.nextTagNumber(&convOkOrCtxt);
//    if (ret == 0 && convOkOrCtxt) {//we have a first device range - there is a need to have the other tag in this case
//        ret = bParser.parseNext();
//        _rangeLowLimit = bParser.toUInt(&convOkOrCtxt);
//        if (!convOkOrCtxt)
//            return -BacnetReject::ReasonInvalidParameterDataType;
//        consumedBytes += ret;
//        buffLength -= ret;
//        ret = bParser.parseNext();
//        if (ret < 0)
//            return -BacnetReject::ReasonMissingRequiredParameter;
//        _rangeHighLimit = bParser.toUInt(&convOkOrCtxt);
//        if (!convOkOrCtxt)
//            return -BacnetReject::ReasonInvalidParameterDataType;
//        consumedBytes += ret;
//    }

//    if (bParser.hasNext())
//        return -BacnetReject::ReasonTooManyArguments;

//    return consumedBytes;
}
