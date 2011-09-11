#include "readpropertyservice.h"
#include "bacnettagparser.h"

ReadPropertyServiceHandler::ReadPropertyServiceHandler():
        _propId(BacnetProperty::UndefinedProperty),
        _arrayIdx(NO_ARRAY_IDX_SPECIFIED)
{
    _objId.instanceNum = 0;
    _objId.objectType = BacnetObjectType::Undefined;
}

qint32 ReadPropertyServiceHandler::fromRaw(quint8 *servicePtr, quint16 length)
{
    quint8 *actualPtr = servicePtr;
    qint32 ret(0);
    bool convOk;

    BacnetTagParser bParser(servicePtr, length);
    //get object id
    ret = bParser.parseNext();
    if (ret <= 0 || !bParser.isContextTag() || bParser.tagNumber() != 0) {//not enough data, not context tag or not this tag
        return -1;
    }
    actualPtr += ret;
    _objId = bParser.toObjectId(&convOk);
    if (!convOk) {
        return -1;
    }
    //get property identifier
    ret = bParser.parseNext();
    if (ret <= 0 || !bParser.isContextTag() || bParser.tagNumber()!=1) {//not enough data, not context tag or not this tag
        return -1;
    }
    actualPtr += ret;
    _propId = (BacnetProperty::Identifier)bParser.toInt(&convOk);
    if (!convOk) {
        return -1;
    }
    //get atrray index - optional
    ret = bParser.parseNext();
    if (ret < 0) {//equal 0 means we didn't get
        return -1;
    } else if (0 != ret) {
        if ((!bParser.isContextTag() || bParser.tagNumber()!=2)) {//if there is another tag, but is not context spec. or is not 2 - error
            return -1;
        }
        //there is an array index specified
        actualPtr += ret;
        _arrayIdx = bParser.toUInt(&convOk);
        if (!convOk) {
            return -1;
        }
    }
    //no more needed to be parsed. The property value is object specific and will be extracted when executed,
    Q_ASSERT((actualPtr - servicePtr) == length);//the length shouldn't be more than the service
    return actualPtr - servicePtr;
}
