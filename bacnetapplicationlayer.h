#ifndef BACNETAPPLICATIONLAYERHANDLER_H
#define BACNETAPPLICATIONLAYERHANDLER_H

#include <QtCore>
#include <QList>
#include <QObject>

#include "bacnetpci.h"
#include "routingtable.h"
#include "remoteobjectstodevicemapper.h"
#include "bacnettsm2.h"
#include "externalconfirmedservicewrapper.h"

class BacnetAddress;
class BacnetNetworkLayerHandler;
class InternalObjectsHandler;

namespace Bacnet {
class ExternalObjectsHandler;
class ExternalConfirmedServiceHandler;
class BacnetServiceData;
class BacnetDeviceObject;
class DiscoveryWrapper;
class Error;
class ObjectIdentifier;

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
    void processAbort(BacnetAddress &remoteSource, BacnetAddress &localDestination, ExternalConfirmedServiceHandler *serviceAct);
    void processReject(BacnetAddress &remoteSource, BacnetAddress &localDestination, BacnetRejectNS::RejectReason reason, ExternalConfirmedServiceHandler *serviceAct);
    void processError(BacnetAddress &remoteSource, BacnetAddress &localDestination, Error &error, ExternalConfirmedServiceHandler *serviceAct);
    void processTimeout(BacnetAddress &remoteDestination, BacnetAddress &localSource, ExternalConfirmedServiceHandler *serviceAct);

    //! tries to send the data. When there is no translation entry obj id -> address, returns false. When discovery is to be used, when such case happens, use \sa sendUnconfirmedWithDiscovery()
    bool sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);
    //! sends unconfirmed data. When there is no translation entry objId -> address, first issues Who-has service. \note Takes ownership over BacnetServiceData.
    bool sendUnconfirmedWithDiscovery(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData *data, quint8 serviceChoice);

    void sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);
    bool send(const ObjectIdStruct &destinedObject, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);
    bool send(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);
    inline void sendAck(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetServiceData *data, BacnetConfirmedRequestData *reqData) {_tsm->sendAck(remoteDestination, localSource, data, reqData);}
    inline void sendReject(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetRejectNS::RejectReason reason, quint8 invokeId) {_tsm->sendReject(remoteDestination, localSource, reason, invokeId);}
    inline void sendError(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetServicesNS::BacnetErrorChoice errorChoice, Error &error) {_tsm->sendError(remoteDestination, localSource, invokeId, errorChoice, error);}
    inline void sendAbort(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetAbortNS::AbortReason abortReason, bool fromServer) {_tsm->sendAbort(remoteDestination, localSource, invokeId, abortReason, fromServer);}


    InternalObjectsHandler *internalHandler();
    ExternalObjectsHandler *externalHandler();
    QList<BacnetDeviceObject*> devices();

private:
    /** Sends discovery request for objectId.
    If the object Id is of device type, Who-Is request is submitted. Otherwise Who-has service request is sent.
      */
    friend class UnconfirmedDiscoveryWrapper;
    friend class ConfirmedDiscoveryWrapper;
    void discover(quint32 objectId, bool forceToHave = false);
    QHash<ObjIdNum, DiscoveryWrapper*> _awaitingDiscoveries;
public:
    void registerObject(BacnetAddress &devAddress, ObjectIdentifier &devId, ObjectIdentifier &objId, QString &objName);
    void registerDevice(BacnetAddress &devAddress, ObjectIdentifier &devId, quint32 maxApduSize, BacnetSegmentation segmentationType, quint32 vendorId);

private:
    QHash<ExternalConfirmedServiceHandler*, ExternalConfirmedServiceWrapper> _awaitingConfirmedServices;
    void cleanUpService(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 action, QHash<ExternalConfirmedServiceHandler*, ExternalConfirmedServiceWrapper>::Iterator &it);

protected:
    void timerEvent(QTimerEvent *);

protected:
    BacnetNetworkLayerHandler *_networkHndlr;

public:
    InternalObjectsHandler *_internalHandler;
    Bacnet::ExternalObjectsHandler *_externalHandler;
    Bacnet::BacnetTSM2 *_tsm;

private:
    static const int TimerInterval_ms = 1000;
    QBasicTimer _timer;
    static const int DefaultDynamicElementsSize = 100;
    RoutingTable _devicesRoutingTable;
    static const int DefaultMapperElementsSize = 100;
    RemoteObjectsToDeviceMapper _objectDeviceMapper;

};

}

#endif // BACNETAPPLICATIONLAYERHANDLER_H
