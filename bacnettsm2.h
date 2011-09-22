#ifndef BACNETTSM2_H
#define BACNETTSM2_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetcommon.h"
#include "bacnetservicedata.h"
#include "bacnetinternaladdresshelper.h"

class BacnetSimpleAckData;
class BacnetComplexAckData;
class BacnetSegmentedAckData;
class BacnetErrorData;
class BacnetAbortData;
class BacnetRejectData;
class BacnetNetworkLayerHandler;

namespace Bacnet {

    class BacnetConfirmedServiceHandler;
    class AsynchronousBacnetTsmAction;
    class Error;

    class BacnetTSM2:
            public QObject
    {
        Q_OBJECT
    public:
        explicit BacnetTSM2(QObject *parent = 0);

//        bool sendAction(BacnetAddress &receiver, AsynchronousBacnetTsmAction *actionToSend);

        bool send(ObjectIdStruct &destinedObject, InternalAddress &sourceAddress, BacnetServices::BacnetConfirmedServiceChoice service, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms = 1000);

        void receive(BacnetAddress &source, BacnetAddress &destination, BacnetSimpleAckData *data);
        void receive(BacnetAddress &source, BacnetAddress &destination, BacnetComplexAckData *data, quint8 *bodyPtr, quint16 bodyLength);
        void receive(BacnetAddress &source, BacnetAddress &destination, BacnetSegmentedAckData *data, quint8 *bodyPtr, quint16 bodyLength);
        void receive(BacnetAddress &source, BacnetAddress &destination, BacnetErrorData *data, quint8 *bodyPtr, quint16 bodyLength);
        void receive(BacnetAddress &source, BacnetAddress &destination, BacnetRejectData *data, quint8 *bodyPtr, quint16 bodyLength);
        void receive(BacnetAddress &source, BacnetAddress &destination, BacnetAbortData *data, quint8 *bodyPtr, quint16 bodyLength);


        void sendAck(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData *data, quint8 invokeId, quint8 serviceChoice);
        void sendReject(BacnetAddress &destination, BacnetAddress &source, BacnetReject::RejectReason reason, quint8 invokeId);
        void sendError(BacnetAddress &destination, BacnetAddress &source, quint8 invokeId, BacnetServices::BacnetErrorChoice errorChoice, Error &error);
        void sendUnconfirmed(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);

        void setAddress(InternalAddress &address);
        InternalAddress &myAddress();


    public:
        bool deviceAddress(ObjectIdStruct &deviceId, BacnetAddress *address);
        void discoverDevice(ObjectIdStruct &deviceId);

    private:
        struct ConfirmedRequestEntry
        {
            //invoke id is a key
            BacnetConfirmedServiceHandler *handler;
            quint32 timeLeft_ms;
        };

        struct ConfirmedAwaitingDiscoveryEntry
        {
            //objId is a key
            BacnetConfirmedServiceHandler *handler;
            BacnetServices::BacnetConfirmedServiceChoice choice;
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
