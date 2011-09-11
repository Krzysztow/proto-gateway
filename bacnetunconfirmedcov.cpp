#include "bacnetunconfirmedcov.h"

#include "bacnettagparser.h"

using namespace Bacnet;

BacnetUnconfirmedCov::BacnetUnconfirmedCov()
{
}

BacnetUnconfirmedCov::~BacnetUnconfirmedCov()
{
}

//! \todo Remove this duplication with BacnetConfirmedCov - either inherit, compose or make satic function.
qint16 BacnetUnconfirmedCov::parseFromRaw(quint8 *servicePtr, quint16 length)
{
    quint8 *actualPtr(servicePtr);
    qint16 ret(0);
    bool convOk;

    BacnetTagParser bParser(servicePtr, length);

    //get process id
    ret = bParser.parseNext();
    _subscribProcess = bParser.toUInt(&convOk);
    if (ret <= 0 || !bParser.isContextTag(0) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get object identifier
    ret = bParser.parseNext();
    _initiatingObjectId = bParser.toObjectId(&convOk);
    if (ret <= 0 || !bParser.isContextTag(1) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get object identifier
    ret = bParser.parseNext();
    _monitoredObjectId = bParser.toObjectId(&convOk);
    if (ret <= 0 || !bParser.isContextTag(2) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    //get time remaining
    ret = bParser.parseNext();
    _timeLeft = bParser.toUInt(&convOk);
    if (ret <= 0 || !bParser.isContextTag(3) || !convOk) //not enough data, not context tag or not this tag
        return -1;
    actualPtr += ret;

    ret = _listOfValues.fromRawSpecific(bParser, 4, _monitoredObjectId.objectType);
    if (ret <= 0)
        return -2;

    actualPtr += ret;

    //no more needed to be parsed. The property value is object specific and will be extracted when executed,
    return actualPtr - servicePtr;
}

//#define UNCONFIRMED_COV_TEST
#ifdef UNCONFIRMED_COV_TEST
int main()
{
    quint8 unconfCov[] = {
        0x09,
        0x12,
        0x1C,
        0x02, 0x00, 0x00, 0x04,
        0x2C,
        0x00, 0x00, 0x00, 0x0A,
        0x39,
        0x00,
        0x4E,
        0x09,
        0x55,
        0x2E,
        0x44,
        0x42, 0x82, 0x00, 0x00,
        0x2F,
        0x09,
        0x6F,
        0x2E,
        0x82,
        0x04, 0x00,
        0x2F,
        0x4F
    };

    BacnetUnconfirmedCov covServ;
    qint32 ret = covServ.parseFromRaw(unconfCov, sizeof(unconfCov));

    return 0;
}
#endif
