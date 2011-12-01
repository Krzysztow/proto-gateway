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
    ExternalConfirmedServiceHandler *dequeueConfirmedRequest(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 invokeId);
    BacnetApplicationLayerHandler *_appLayer;

public:
    bool send(const BacnetAddress &destination, BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend);

    void sendAck(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetServiceData *data, BacnetConfirmedRequestData *reqData);
    void sendReject(BacnetAddress &remoteDestination, BacnetAddress &localSource, BacnetRejectNS::RejectReason reason, quint8 invokeId);
    void sendError(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetServicesNS::BacnetErrorChoice errorChoice, Error &error);
    void sendAbort(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId, BacnetAbortNS::AbortReason abortReason, bool fromServer);

    void sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice);

    void setAddress(InternalAddress &address);
    InternalAddress &myAddress();

private:
    bool send_hlpr(const BacnetAddress &destination, BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend, quint8 invokeId);


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
        ConfirmedRequestEntry(ExternalConfirmedServiceHandler *handler, int timeout_ms, int retriesNum, const BacnetAddress &destination, const BacnetAddress &source);

    public:
        ExternalConfirmedServiceHandler *handler;
        int timeLeft_ms;
        int retriesLeft;
        BacnetAddress dst;
        BacnetAddress src;
    };
    int queueConfirmedRequest(ExternalConfirmedServiceHandler *handler, const BacnetAddress &destination, const BacnetAddress &source);
    QHash<int, ConfirmedRequestEntry> _confiremedEntriesList;

    QBasicTimer _timer;
    static const int DefaultTimerInterval_ms = 250;
    int _timerInterval_ms;

private:
    InvokeIdGenerator _generator;
    InternalAddress _myRequestAddress;
    BacnetNetworkLayerHandler *_netHandler;
};

}

#endif // BACNETTSM2_H
