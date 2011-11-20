#include "whohasservicedata.h"

#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

WhoHasServiceData::WhoHasServiceData():
        _rangeLowLimit(Bacnet::InvalidInstanceNumber),
        _rangeHighLimit(Bacnet::InvalidInstanceNumber),
        _objidentifier(0),
        _objName(0)
{
}

WhoHasServiceData::~WhoHasServiceData()
{
    delete _objidentifier;
    delete _objName;
}

qint32 WhoHasServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    Q_CHECK_PTR(startPtr);
    qint32 ret(0);
    quint8 *actualPtr(startPtr);

    //encode
    if (_rangeLowLimit != Bacnet::InvalidInstanceNumber || _rangeHighLimit != Bacnet::InvalidInstanceNumber) {
        Q_ASSERT(_rangeLowLimit != Bacnet::InvalidInstanceNumber && _rangeHighLimit != Bacnet::InvalidInstanceNumber);
        if (_rangeLowLimit == Bacnet::InvalidInstanceNumber || _rangeHighLimit == Bacnet::InvalidInstanceNumber) {//wrong!
            return -1;//either both or none!
        }

        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _rangeLowLimit, true, 0);
        if (ret < 0)
            return -1;
        actualPtr += ret;
        buffLength -= ret;

        ret = BacnetCoder::uintToRaw(actualPtr, buffLength, _rangeHighLimit, true, 1);
        if (ret < 0)
            return -2;
        actualPtr += ret;
        buffLength -= ret;
    }

    //encode one of the choices
    if (0 != _objidentifier) {
        ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, (*_objidentifier), true, 2);
    } else if (0 != _objName) {
        ret = BacnetCoder::stringToRaw(actualPtr, buffLength, (*_objName), true, 3);
    } else
        return -3;

    if (ret < 0)
        return -3;
    actualPtr += ret;

    return (actualPtr - startPtr);
}

qint32 WhoHasServiceData::fromRaw(quint8 *serviceData, quint16 buffLength)
{    
    Q_CHECK_PTR(serviceData);
    BacnetTagParser bParser(serviceData, buffLength);

    qint16 ret;
    qint16 consumedBytes(0);
    bool convOkOrCtxt;

    //check for object identifiers
    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if (ret == 0 && convOkOrCtxt) {//we have a first device range - there is a need to have the other tag in this case
        ret = bParser.parseNext();
        _rangeLowLimit = bParser.toUInt(&convOkOrCtxt);
        if (!convOkOrCtxt)
            return -BacnetRejectNS::ReasonInvalidParameterDataType;
        consumedBytes += ret;
        buffLength -= ret;
        ret = bParser.parseNext();
        if (ret < 0)
            return -BacnetRejectNS::ReasonMissingRequiredParameter;
        _rangeHighLimit = bParser.toUInt(&convOkOrCtxt);
        if (!convOkOrCtxt)
            return -BacnetRejectNS::ReasonInvalidParameterDataType;
        consumedBytes += ret;
    } else {
        _rangeLowLimit = Bacnet::InvalidInstanceNumber;
        _rangeHighLimit = Bacnet::InvalidInstanceNumber;

    }

    //get object identifier or name - there must be one of them!
    ret = bParser.parseNext();
    if (ret < 0)
        return -BacnetRejectNS::ReasonInvalidTag;
    if (bParser.isContextTag(2)) {//we have an object ID
        _objidentifier = new ObjectIdStruct;
        *(_objidentifier) = bParser.toObjectId(&convOkOrCtxt);
        if (!convOkOrCtxt)
            return -BacnetRejectNS::ReasonInvalidParameterDataType;
        consumedBytes += ret;
    } else if (bParser.isContextTag(3)){ //we have an object name
        _objName = new QString();
        *_objName = bParser.toString(&convOkOrCtxt);
        if (!convOkOrCtxt)
            return -BacnetRejectNS::ReasonInvalidParameterDataType;
        consumedBytes += ret;
    } else
        return -BacnetRejectNS::ReasonMissingRequiredParameter;

    if (bParser.hasNext())
        return -BacnetRejectNS::ReasonTooManyArguments;

    return consumedBytes;
}
