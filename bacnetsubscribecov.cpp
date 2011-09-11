#include "bacnetsubscribecov.h"

#include "bacnettagparser.h"

using namespace Bacnet;

BacnetSubscribeCov::BacnetSubscribeCov()
{
}

BacnetSubscribeCov::~BacnetSubscribeCov()
{

}

qint16 BacnetSubscribeCov::parseFromRaw(quint8 *servicePtr, quint16 length)
{
    qint32 total(0);
    qint32 ret(0);
    bool convOkOrCtxt;

    BacnetTagParser bParser(servicePtr, length);

    //parse subscriber; 0 to unsubscribe.
    ret = bParser.parseNext();
    _subscrProcessId = bParser.toUInt(&convOkOrCtxt);
    if ( (ret<0) || !bParser.isContextTag(0) || !convOkOrCtxt)
        return -1;
    total += ret;
    //parse momonitored object identifier
    ret = bParser.parseNext();
    _monitoredObjId = bParser.toObjectId(&convOkOrCtxt);
    if ( (ret<0) || !bParser.isContextTag(1) || !convOkOrCtxt)
        return -2;
    total += ret;
    /*If both the ,IssueConfirmed Notifications, and ,Lifetime, parameters are absent, then this shall
    indicate a cancellation request. If the ,Lifetime, parameter is present then the ,Issue Confirmed
    Notifications, parameter shall be present.
     */
    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if ( (2 == ret) && convOkOrCtxt) {//is there an Issue Confirmed Notifications
        ret = bParser.parseNext();
        _issueConfirmedNotifs = bParser.toBoolean(&convOkOrCtxt);
        if (ret < 0 || !convOkOrCtxt)
            return -3;
        total += ret;
    } else {
        _issueConfirmedNotifs = false;
    }

    ret = bParser.nextTagNumber(&convOkOrCtxt);
    if ( (3 == ret) && convOkOrCtxt) {//there is a lfetime parameter
        ret = bParser.parseNext();
        _lifetime = bParser.toUInt(&convOkOrCtxt);
        if (ret < 0 || !convOkOrCtxt)
            return -4;
        total += ret;
    }
    else {
        _lifetime = 0;
    }

    return total;
}


//#define CONFIRMED_COV_TEST
#ifdef CONFIRMED_COV_TEST
int main()
{
    quint8 subscribeCovdata[] = {
        0x09,
        0x12,
        0x1C,
        0x00, 0x00, 0x00, 0x0A,
        0x29,
        0x01,
        0x39,
        0x00
    };

    BacnetSubscribeCov sCov;
    qint32 ret = sCov.parseFromRaw(subscribeCovdata, sizeof(subscribeCovdata));

    //to make following checks, comment private keyword in the class definition!
    Q_ASSERT(ret >= 0);
    Q_ASSERT(18 == sCov._subscrProcessId);
    Q_ASSERT (BacnetObjectType::AnalogInput == sCov._monitoredObjId.objectType);
    Q_ASSERT (0x0a == sCov._monitoredObjId.instanceNum);
    Q_ASSERT(true == sCov._issueConfirmedNotifs);
    Q_ASSERT(0 == sCov._lifetime);

    qDebug("Read number of bytes: %d", ret);

    return 0;
}
#endif
