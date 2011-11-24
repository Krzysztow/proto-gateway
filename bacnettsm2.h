#ifndef BACNETTSM2_H
#define BACNETTSM2_H

#include <QObject>
#include <QBasicTimer>

#include "bacnetaddress.h"
#include "bacnetcommon.h"
#include "bacnetservicedata.h"
#include "bacnetinternaladdresshelper.h"
#include "bacnetpci.h"
#include "invokeidgenerator.h"

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
private:
    ExternalConfirmedServiceHandler *takeRespondedService(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 invokeId);
    BacnetApplicationLayerHandler *_appLayer;

public:
//    bool send(const ObjectIdStruct &destinedObject, InternalAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);
    bool send(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend);

    void sendAck(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetServiceData *data, BacnetConfirmedRequestData *reqData);
    void sendReject(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetRejectNS::RejectReason reason, quint8 invokeId);
    void sendError(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetServicesNS::BacnetErrorChoice errorChoice, Error &error);
    void sendAbort(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetAbortNS::AbortReason abortReason, bool fromServer);

//    void sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);
    void sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);

    void setAddress(InternalAddress &address);
    InternalAddress &myAddress();

private:
    bool send_hlpr(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServicesNS::BacnetConfirmedServiceChoice service, ExternalConfirmedServiceHandler *serviceToSend, quint8 invokeId);

public:
    bool deviceAddress(const ObjectIdStruct &deviceId, BacnetAddress *address);
    void discoverDevice(const ObjectIdStruct &deviceId);

protected:
    void timerEvent(QTimerEvent *);

private:
    static const int DefaultTimeout_ms = 1000;
    static const int DefaultRetryCount = 3;
    int _requestTimeout_ms;
    int _requestRetriesCount;
    class ConfirmedRequestEntry
    {
    public:
        //invoke id is a key
        ConfirmedRequestEntry(ExternalConfirmedServiceHandler *handler, int timeout_ms, int retriesNum, const BacnetAddress &destination, const BacnetAddress &source, BacnetServicesNS::BacnetConfirmedServiceChoice serviceCode);

    public:
        ExternalConfirmedServiceHandler *handler;
        int timeLeft_ms;
        int retriesLeft;
        BacnetAddress dst;
        BacnetAddress src;
        BacnetServicesNS::BacnetConfirmedServiceChoice service;
    };
    int queueConfirmedRequest(ExternalConfirmedServiceHandler *handler, const BacnetAddress &destination, const BacnetAddress &source, BacnetServicesNS::BacnetConfirmedServiceChoice service);
    QHash<int, ConfirmedRequestEntry> _confiremedEntriesList;

    QBasicTimer _timer;
    static const int DefaultTimerInterval_ms = 250;
    int _timerInterval_ms;

    struct ConfirmedAwaitingDiscoveryEntry
    {
        //objId is a key
        ExternalConfirmedServiceHandler *handler;
        BacnetServicesNS::BacnetConfirmedServiceChoice choice;
        InternalAddress sourceAddress;
        quint32 timeLeft_ms;
    };

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
        quint16 timeLeft_ms;

        bool isInitialized() {return (type & Initialized);}
        bool hasExpired() { return ((type & DynamicExpiring) == 0 ? false : (timeLeft_ms == 0));}
    };
    QHash<ObjectIdStruct, RoutingEntry> _routingTable;

signals:

public slots:
    //each interval, check pendingConfirmedRequests, awaitingDiscoveryRequests and check RoutingElements

    //Invoke Id GENERATOR
private:
    InvokeIdGenerator _generator;
};

}

#endif // BACNETTSM2_H
