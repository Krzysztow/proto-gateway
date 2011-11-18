#include "bacnetdata.h"

#include "bacnetcoder.h"
#include "bacnettagparser.h"
#include "bacnetdefaultobject.h"

using namespace Bacnet;

BacnetDataInterface::BacnetDataInterface()
{
}

BacnetDataInterface::~BacnetDataInterface()
{}

qint32 BacnetDataInterface::toRawTagEnclosed_helper(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    qint32 ret;
    qint32 total(0);
    quint8 *actualPtr(ptrStart);

    ret = BacnetCoder::openingTagToRaw(actualPtr, buffLength, tagNumber);
    if (ret < 0)
        return -1;
    total += ret;
    actualPtr += ret;
    buffLength -= ret;
    ret = toRaw(actualPtr, buffLength);
    if (ret < 0)
        return -2;
    actualPtr += ret;
    total += ret;
    buffLength -= ret;
    ret = BacnetCoder::closingTagToRaw(actualPtr, buffLength, tagNumber);
    if (ret<0)
        return -3;

    total += ret;
    return total;
}

qint32 BacnetDataInterface::fromRawTagEnclosed_helper(BacnetTagParser &parser, quint8 tagNum)
{
    qint32 total(0);
    qint32 ret;

    ret = parser.parseNext();
    if (ret <0 || !parser.isOpeningTag(tagNum))
        return -1;
    total += ret;
    ret = fromRaw(parser);
    if (ret<0)
        return -2;
    total += ret;
    ret = parser.parseNext();
    if (ret<0 || !parser.isClosingTag(tagNum))
        return -3;
    total += ret;

    return total;
}

qint32 BacnetDataInterface::fromRawChoiceValue_helper(BacnetTagParser &parser, QList<DataType::DataType> choices, Bacnet::BacnetDataInterface *choiceValue)
{
    Q_CHECK_PTR(choiceValue);
    Q_ASSERT(!choices.isEmpty());

    qint32 ret;
    bool isContext;
    ret = parser.nextTagNumber(&isContext);
    if (ret < 0 || ret >= choices.size())
        return -1;

    choiceValue = BacnetDefaultObject::createDataType(choices.at(ret));
    Q_CHECK_PTR(choiceValue);
    return choiceValue->fromRaw(parser);
}

DEFINE_VISITABLE_FUNCTION(BacnetDataInterface)

BacnetDataInterface *BacnetDataInterface::getValue(qint32 arrayIndex)
{
    if (arrayIndex == ArrayIndexNotPresent)
        return this;
    return 0;
}
