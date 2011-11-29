#ifndef BACNET_INTERNALCONFIRMEDCOVNOTIFHANDLER_H
#define BACNET_INTERNALCONFIRMEDCOVNOTIFHANDLER_H

#include "internalconfirmedrequesthandler.h"
#include "covnotificationrequestdata.h"

namespace Bacnet {

class Error;

class InternalConfirmedCovNotifHandler:
        public InternalConfirmedRequestHandler
{
public:
    InternalConfirmedCovNotifHandler(BacnetConfirmedRequestData *crData, BacnetAddress &requester, BacnetAddress &destination,
                                     BacnetApplicationLayerHandler *appLayer);
    virtual ~InternalConfirmedCovNotifHandler();

public://methods overridden from InternalRequestHandler
    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device);
    virtual bool isFinished();
    virtual void finalize(bool *deleteAfter);
    virtual bool execute();
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

public:
    virtual bool hasError();
    virtual Bacnet::Error &error();
    virtual Bacnet::BacnetServiceData *takeResponseData();

private:
    BacnetApplicationLayerHandler *_appLayer;

    CovNotificationRequestData _data;
    Error _error;
};

} // namespace Bacnet

#endif // BACNET_INTERNALCONFIRMEDCOVNOTIFHANDLER_H
