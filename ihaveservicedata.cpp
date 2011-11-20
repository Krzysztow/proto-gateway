#include "ihaveservicedata.h"

#include "bacnetcoder.h"
#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

IHaveServiceData::IHaveServiceData(ObjectIdentifier &devId, ObjectIdentifier &objId, QString objName):
        _devId(devId),
        _objId(objId),
        _objName(objName)
{
}

IHaveServiceData::IHaveServiceData()
{
}

qint32 IHaveServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    qint32 ret(0);
    quint8 *actualPtr(startPtr);

    //encode device id
    ret = _devId.toRaw(actualPtr, buffLength);
    if (ret < 0)
        return ret;
    actualPtr += ret;
    buffLength -= ret;

    //encode object id
    ret = _objId.toRaw(actualPtr, buffLength);
    if (ret < 0)
        return -2;
    actualPtr += ret;
    buffLength -= ret;

    //encode name
    ret = BacnetCoder::stringToRaw(actualPtr, buffLength, _objName, false, AppTags::CharacterString);
    if (ret < 0)
        return -3;
    actualPtr += ret;
    buffLength -= ret;

    return (actualPtr - startPtr);
}

qint32 IHaveServiceData::fromRaw(quint8 *serviceData, quint16 buffLength)
{
    Q_CHECK_PTR(serviceData);
    BacnetTagParser bParser(serviceData, buffLength);

    qint16 ret;
    qint16 consumedBytes(0);
    bool convOk;

    //parse object identifier
    ret = _devId.fromRaw(bParser);
    if (ret < 0)
        return -BacnetRejectNS::ReasonMissingRequiredParameter;
    consumedBytes += ret;

    //parse object identifier
    ret = _objId.fromRaw(bParser);
        return -BacnetRejectNS::ReasonMissingRequiredParameter;
    consumedBytes += ret;

    //parse object name
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isContextTag(AppTags::CharacterString))
        return -BacnetRejectNS::ReasonMissingRequiredParameter;
    _objName = bParser.toString(&convOk);
    if (!convOk)
        return -BacnetRejectNS::ReasonInvalidParameterDataType;

    //if something is left - wrong!
    if (bParser.hasNext())
        return -BacnetRejectNS::ReasonTooManyArguments;

    return consumedBytes;
}
