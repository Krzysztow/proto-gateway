#ifndef BACNETAPPLICATIONLAYERHANDLER_H
#define BACNETAPPLICATIONLAYERHANDLER_H

#include <QtCore>
#include <QList>
#include <QObject>

#include "bacnetpci.h"

class BacnetAddress;
class BacnetNetworkLayerHandler;
class InternalObjectsHandler;

namespace Bacnet {
class ExternalObjectsHandler;
class ExternalConfirmedServiceHandler;
class BacnetTSM2;
class BacnetServiceData;
class BacnetDeviceObject;

class BacnetApplicationLayerHandler:
        public QObject
{
    Q_OBJECT
public:
    BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr, QObject *parent = 0);
    virtual ~BacnetApplicationLayerHandler();

    /**
      Sets the the network layer handler, that will be invoked whenever new message is to be sent.
      */
    void setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr);

    /**
      This is a method that should be invoked by Network Layer in order to pass data (starting at actualBytePtr*) of
      lenght length.
      \param - sourceAddr - is used by some requests/responses to localize the other device;
      \param - destAddr - not specified by BACnet but if application layer is supposed to act as a collection of
      devices we need to have information which device is being called.
      */
    void indication(quint8 *data, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr);

    void processConfirmedRequest(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, BacnetConfirmedRequestData *crData);
    void processUnconfirmedRequest(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, BacnetUnconfirmedRequestData &ucrData);
    void processAck(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct);
    void processError(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct);
    void processReject(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct);
    void processAbort(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *dataPtr, quint16 dataLength, ExternalConfirmedServiceHandler *serviceAct);
    void processTimeout(ExternalConfirmedServiceHandler *serviceAct);

    void sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);
    void sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);
    bool send(const ObjectIdStruct &destinedObject, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);
    bool send(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);


    InternalObjectsHandler *internalHandler();
    ExternalObjectsHandler *externalHandler();
    QList<BacnetDeviceObject*> devices();

protected:
    BacnetNetworkLayerHandler *_networkHndlr;

    //! \todo should be changed when configuration is done with config files.
public:
    InternalObjectsHandler *_internalHandler;
    Bacnet::ExternalObjectsHandler *_externalHandler;
    Bacnet::BacnetTSM2 *_tsm;
};

}

#endif // BACNETAPPLICATIONLAYERHANDLER_H
