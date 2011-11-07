#include "bacnetdataabstract.h"

#include "bacnetcoder.h"
#include "bacnettagparser.h"

using namespace Bacnet;

DataAbstract::DataAbstract()
{
}

qint32 DataAbstract::toRaw(quint8 *ptrStart, quint16 buffLength)
{
    Q_UNUSED(ptrStart); Q_UNUSED(buffLength);
    Q_ASSERT_X(false, "DataAbstract", "Are you sure you want to use it for writing data?");
    return BacnetCoder::UnknownError;
    //or else
//    if (buffLength < _data.size())
//        return BacnetCoder::BufferOverrun;

//    memcpy(ptrStart, _data.data(), _data.size());
//    return _data.size();
}

qint32 DataAbstract::toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber)
{
    Q_UNUSED(ptrStart); Q_UNUSED(buffLength); Q_UNUSED(tagNumber);
    Q_ASSERT_X(false, "DataAbstract", "Are you sure you want to use it for writing data?");
    return BacnetCoder::UnknownError;
    //or else
//    if (buffLength < _data.size())
//        return BacnetCoder::BufferOverrun;

//    memcpy(ptrStart, _data.data(), _data.size());
//    return _data.size();
}

qint32 DataAbstract::fromRaw(BacnetTagParser &parser)
{
    Q_ASSERT_X(false, "DataAbstract::fromRaw()",
               "DataAbstract::fromRaw() - this class should be only used for constructed datatypes.");
    return -1;
}

qint32 DataAbstract::fromRaw(BacnetTagParser &parser, quint8 tagNumber)
{
    //we parse from the opening tag to the closing one.
    qint32 ret;
    quint8 depth(1);//this stores the number of opening tags with the same tagNumber as ours - not to be lost.

    quint8 *dataStart(0);

    ret = parser.parseNext();
    if (ret < 0 || !parser.isOpeningTag(tagNumber))
        return -1;

    dataStart = parser.actualTagStart();

    //now we know the opening tag is equal to the tagNumber;
    while (!parser.isClosingTag(tagNumber) || (0 != depth)) {
        ret = parser.parseNext();
        if (ret < 0)
            return -2;
        if (parser.isOpeningTag(tagNumber))
            ++depth;
        else if (parser.isClosingTag(tagNumber))
            --depth;
    }

    //means we are at the closing tag
    quint32 dataLength = parser.actualTagStart() - dataStart + parser.actualTagAndDataLength();
    _value = QByteArray((const char*)dataStart, dataLength);//make deep copy

    return dataLength;
}

bool DataAbstract::setInternal(QVariant &value)
{
    Q_ASSERT_X(false, "DataAbstract", "No general way to convert variant to abstract data.");
    return false;
}

QVariant DataAbstract::toInternal()
{
    Q_ASSERT_X(false, "DataAbstract", "No general way to convert abstract data to variant!");
    return QVariant();
}

DataType::DataType DataAbstract::typeId()
{
    return DataType::BacnetAbstract;
}

QByteArray &DataAbstract::data()
{
    return _value;
}
