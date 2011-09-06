#include "bacnetreadpropertyack.h"

#include <QtCore>

#include "bacnetdata.h"
#include "bacnetcoder.h"

BacnetReadPropertyAck::BacnetReadPropertyAck():
        _data(0)
{
}

BacnetReadPropertyAck::~BacnetReadPropertyAck()
{
    delete _data;
}

void BacnetReadPropertyAck::setData(Bacnet::BacnetDataInterface *data, Bacnet::ReadPropertyStruct &ackReadPrptyData)
{
    Q_CHECK_PTR(data);
    Q_ASSERT(0 == _data);
    delete _data;//just in case - is it really needed?
    _data = data;
    _value = ackReadPrptyData;
}

qint32 BacnetReadPropertyAck::toRaw(quint8 *startPtr, quint16 buffLength)
{
    quint8 *actualPtr = startPtr;
    quint16 leftLength = buffLength;
    qint32 ret;

    //encode Object identifier
    ret = BacnetCoder::objectIdentifierToRaw(actualPtr, leftLength, _value.objId, true, 0);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode objId");
        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: objId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode proeprty identifier
    ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.propertyId, true, 1);
    if (ret <= 0) {//something wrong?
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode propId");
        qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: propId : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;
    //encode array index if present - OPTIONAL
    if (Bacnet::ArrayIndexNotPresent != _value.arrayIndex) {//present
        ret = BacnetCoder::uintToRaw(actualPtr, leftLength, _value.arrayIndex, true, 2);
        if (ret <= 0) {//something wrong?
            Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode arrayIndex");
            qDebug("BacnetReadPropertyAck::toRaw() : propertyencoding problem: arrayIndex : %d", ret);
            return ret;
        }
        actualPtr += ret;
        leftLength -= ret;
    }

    //encode value
    ret = BacnetCoder::openingTagToRaw(actualPtr, leftLength, 3);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode opening tag");
        qDebug("BacnetReadPropertyAck::toRaw() : cannot encode opening tag: : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    Q_CHECK_PTR(_data);
    ret = _data->toRaw(actualPtr, leftLength);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode abstract data");
        qDebug("BacnetReadPropertyAck::toRaw() : cannot encode abstract data: : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    ret = BacnetCoder::closingTagToRaw(actualPtr, leftLength, 3);
    if (ret <= 0) {
        Q_ASSERT_X(false, "BacnetReadPropertyAck::toRaw()", "Cannot encode closing tag");
        qDebug("BacnetReadPropertyAck::toRaw() : cannot encode closing tag: : %d", ret);
        return ret;
    }
    actualPtr += ret;
    leftLength -= ret;

    return actualPtr - startPtr;
}
