#include "ihaveservicedata.h"

#include "bacnetcoder.h"
#include "bacnetcommon.h"
#include "bacnettagparser.h"

using namespace Bacnet;

IHaveServiceData::IHaveServiceData(ObjectIdStruct &devId, ObjectIdStruct &objId, QString objName):
        _devId(devId),
        _objId(objId),
        _objName(objName)
{
}

IHaveServiceData::IHaveServiceData()
{
    _devId.objectType = BacnetObjectType::Undefined;
    _objId.objectType = BacnetObjectType::Undefined;
}

qint32 IHaveServiceData::toRaw(quint8 *startPtr, quint16 buffLength)
{
    qint32 ret(0);
    quint8 *actualPtr(startPtr);

    //encode device id
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, _devId, false, AppTags::BacnetObjectIdentifier);
    if (ret < 0)
        return -1;
    actualPtr += ret;
    buffLength -= ret;

    //encode object id
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, buffLength, _objId, false, AppTags::BacnetObjectIdentifier);
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
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::BacnetObjectIdentifier))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _devId = bParser.toObjectId(&convOk);
    if (!convOk)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //parse object identifier
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isApplicationTag(AppTags::BacnetObjectIdentifier))
        return -BacnetReject::ReasonMissingRequiredParameter;
    _objId = bParser.toObjectId(&convOk);
    if (!convOk)
        return -BacnetReject::ReasonInvalidParameterDataType;
    consumedBytes += ret;

    //parse object name
    ret = bParser.parseNext();
    if (ret < 0 || !bParser.isContextTag(AppTags::CharacterString))
        return BacnetReject::ReasonMissingRequiredParameter;
    _objName = bParser.toString(&convOk);
    if (!convOk)
        return -BacnetReject::ReasonInvalidParameterDataType;

    //if something is left - wrong!
    if (bParser.hasNext())
        return -BacnetReject::ReasonTooManyArguments;

    return consumedBytes;
}
