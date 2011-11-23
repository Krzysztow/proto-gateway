#ifndef BACNETTSM2_H
#define BACNETTSM2_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetcommon.h"
#include "bacnetservicedata.h"
#include "bacnetinternaladdresshelper.h"

#include "bacnetpci.h"

#define NO_SEGMENTATION_SUPPORTED

class BacnetSimpleAckData;
class BacnetComplexAckData;
class BacnetSegmentedAckData;
class BacnetErrorData;
class BacnetAbortData;
class BacnetRejectData;
class BacnetNetworkLayerHandler;

namespace Bacnet {

class ExternalConfirmedServiceHandler;
class AsynchronousBacnetTsmAction;
class Error;
class BacnetApplicationLayerHandler;

class BacnetTSM2:
        public QObject
{
    Q_OBJECT
public:
    explicit BacnetTSM2(BacnetApplicationLayerHandler *appLayer, QObject *parent = 0);

public://functions connected with parsing
    void receive(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *data, quint16 dataLength);
    //returns service data poitner to the last service received by receive() function. Remember, the storage is valid only temporarily.
    quint8 *serviceDataPtr(quint16 *serviceDataLength);

    void *takeRespondedService(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 invokeId);
private:
    BacnetApplicationLayerHandler *_appLayer;

public:

    bool send(const ObjectIdStruct &destinedObject, InternalAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);
    bool send(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);

    void receive(BacnetAddress &source, BacnetAddress &destination, BacnetSimpleAckData *data);
    void receive(BacnetAddress &source, BacnetAddress &destination, BacnetComplexAckData *data, quint8 *bodyPtr, quint16 bodyLength);
    void receive(BacnetAddress &source, BacnetAddress &destination, BacnetSegmentedAckData *data, quint8 *bodyPtr, quint16 bodyLength);
    void receive(BacnetAddress &source, BacnetAddress &destination, BacnetErrorData *data, quint8 *bodyPtr, quint16 bodyLength);
    void receive(BacnetAddress &source, BacnetAddress &destination, BacnetRejectData *data, quint8 *bodyPtr, quint16 bodyLength);
    void receive(BacnetAddress &source, BacnetAddress &destination, BacnetAbortData *data, quint8 *bodyPtr, quint16 bodyLength);


    void sendAck(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetServiceData *data, quint8 invokeId, quint8 serviceChoice);
    void sendReject(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetRejectNS::RejectReason reason, quint8 invokeId);
    void sendError(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetServicesNS::BacnetErrorChoice errorChoice, Error &error);
    void sendAbort(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetAbortNS::AbortReason abortReason, bool fromServer);

    void sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);
    void sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);

    void setAddress(InternalAddress &address);
    InternalAddress &myAddress();


public:
    bool deviceAddress(const ObjectIdStruct &deviceId, BacnetAddress *address);
    void discoverDevice(const ObjectIdStruct &deviceId);

private:
    struct ConfirmedRequestEntry
    {
        //invoke id is a key
        ExternalConfirmedServiceHandler *handler;
        quint32 timeLeft_ms;
    };

    struct ConfirmedAwaitingDiscoveryEntry
    {
        //objId is a key
        ExternalConfirmedServiceHandler *handler;
        BacnetServicesNS::BacnetConfirmedServiceChoice choice;
        InternalAddress sourceAddress;
        quint32 timeLeft_ms;
    };

    QHash<int, ConfirmedRequestEntry> _pendingConfirmedRequests;
    QHash<ObjectIdStruct, QList<ConfirmedAwaitingDiscoveryEntry> > _awaitingDiscoveryRequests;
    InternalAddress _myRequestAddress;
    BacnetNetworkLayerHandler *_netHandler;

    class RoutingEntry
    {
    public:
        //ObjectIdStruct deviceId;
        BacnetAddress address;
        enum {
            Static          = 0x01,
            Dynamic         = 0x02,
            DynamicExpiring = 0x04,
            Initialized     = 0x08,
            All             = Static | Dynamic | DynamicExpiring | Initialized
        };
        quint8 type;
        quint16 timeLeft_s;

        bool isInitialized() {return (type & Initialized);}
        bool hasExpired() { return ((type & DynamicExpiring) == 0 ? false : (timeLeft_s == 0));}
    };
    QHash<ObjectIdStruct, RoutingEntry> _routingTable;

signals:

public slots:
    //each interval, check pendingConfirmedRequests, awaitingDiscoveryRequests and check RoutingElements

    //Invoke Id GENERATOR
private:
    class InvokeIdGenerator
    {
    public:
        InvokeIdGenerator();

        int generateId();
        void returnId(quint8 id);

    public:
        static const int MaxIdsNumber = 255;

    private:
        static const int NumberOfOctetsTaken = MaxIdsNumber/8 + ( (MaxIdsNumber%8 == 0) ? 0 : 1);
        quint8 idsBits[NumberOfOctetsTaken];
    } _generator;
};

}

#endif // BACNETTSM2_H
