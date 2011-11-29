#ifndef BACNET_INTERNALUNCFRDMCOVNOTIF_H
#define BACNET_INTERNALUNCFRDMCOVNOTIF_H

#include "internalunconfirmedrequesthandler.h"
#include "covnotificationrequestdata.h"

namespace Bacnet {

class BacnetApplicationLayerHandler;

class InternalUncfrdmCovNotifHandler:
        public InternalUnconfirmedRequestHandler
{
public:
    InternalUncfrdmCovNotifHandler(BacnetApplicationLayerHandler *appLayer);

public://overridden from InternalRequestHandler
    virtual bool asynchActionFinished(int asynchId, int result, Bacnet::BacnetObject *object, Bacnet::BacnetDeviceObject *device);
    virtual bool isFinished();
    virtual void finalize(bool *deleteAfter);
    virtual bool execute();
    virtual qint32 fromRaw(quint8 *servicePtr, quint16 length);

private:
    BacnetApplicationLayerHandler *_appLayer;
    CovNotificationRequestData _data;
};

} // namespace Bacnet

#endif // BACNET_INTERNALUNCFRDMCOVNOTIF_H
