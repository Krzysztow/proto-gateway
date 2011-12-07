#include "bacnettsm2.h"

#include "helpercoder.h"
#include "asynchronousbacnettsmaction.h"
#include "externalconfirmedservicehandler.h"
#include "bacnetnetworklayer.h"
#include "bacnetpci.h"
#include "error.h"
#include "bacnetbuffermanager.h"
#include "whoisservicedata.h"
#include "bacnetapplicationlayer.h"

using namespace Bacnet;

BacnetTSM2::BacnetTSM2(BacnetApplicationLayerHandler *appLayer, BacnetNetworkLayerHandler *netLayer, QObject *parent):
    QObject(parent),
    _appLayer(appLayer),
    _requestTimeout_ms(DefaultTimeout_ms),
    _requestRetriesCount(DefaultRetryCount),
    _timerInterval_ms(DefaultTimerInterval_ms),
    _netHandler(netLayer)
{
    Q_CHECK_PTR(_appLayer);
    Q_CHECK_PTR(_netHandler);
    _timer.start(_timerInterval_ms, this);
}

void BacnetTSM2::setAddress(InternalAddress &address)
{
    _myRequestAddress = address;
}

InternalAddress &BacnetTSM2::myAddress()
{
    return _myRequestAddress;
}

bool BacnetTSM2::send_hlpr(const BacnetAddress &destination, BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend, quint8 invokeId)
{
    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *buffStart = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    BacnetConfirmedRequestData reqData(BacnetConfirmedRequestData::Length_206Octets/*BacnetConfirmedRequestData::Length_1476Octets*/, invokeId, serviceToSend->serviceChoice());
    qint32 ret = reqData.toRaw(buffStart, buffLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        qDebug("BacnetTSM2::send() : couldn't write to buffer (pci), %d.", ret);
        return false;
    }
    buffStart += ret;
    buffLength -= ret;
    ret = serviceToSend->toRaw(buffStart, buffLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        qDebug("BacnetTSM2::send() : couldn't write to buffer, %d.", ret);
        return false;
    }
    buffStart += ret;
    buffer.setBodyLength(buffStart - buffer.bodyPtr());

    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "Request to be sent: ");

    _netHandler->sendApdu(&buffer, true, &destination, &sourceAddress);
    Q_ASSERT(_confiremedEntriesList.contains(invokeId));
    return true;
}

bool BacnetTSM2::send(const BacnetAddress &destination, BacnetAddress &sourceAddress, ExternalConfirmedServiceHandler *serviceToSend)
{
    //generate invoke id.
    int invokeId = queueConfirmedRequest(serviceToSend, destination, sourceAddress);
    if (invokeId < 0) {//can't generate
        qDebug("BacnetTSM2::send() : can't generate invoke id. Think about introducing another requesting object.");
        return false;
    }

    return send_hlpr(destination, sourceAddress, serviceToSend, invokeId);
}

void BacnetTSM2::sendReject(BacnetAddress &destination, BacnetAddress &source, BacnetRejectNS::RejectReason reason, quint8 invokeId)
{
    BacnetRejectData rejectData(invokeId, reason);

    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *buffStart = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    qint32 ret = rejectData.toRaw(buffStart, buffLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        qDebug("BacnetTSM2::sendReject() : couldn't write to buffer (%d)", ret);
        return;
    }
    buffer.setBodyLength(ret);

    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "TSM : Sending reject message with:");
    _netHandler->sendApdu(&buffer, false, &destination, &source);
}


void BacnetTSM2::receive(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 *data, quint16 dataLength)
{
    //handle accordingly to the request type.
    BacnetPci::BacnetPduType pduType = BacnetPci::pduType(data);
    switch (pduType)
    {
    case (BacnetPci::TypeConfirmedRequest):
    {
        /*upon reception:
                  - when no semgenation - do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
                  - when segmented - respond with BacnetSegmentAck PDU and when all gotten, do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
                 */

        BacnetConfirmedRequestData *crData = new BacnetConfirmedRequestData();
        qint32 ret = crData->fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        //! \todo What to send here? If we couldn't parse it we even have no data for reject (invoke id);
        if (ret <= 0) {
            delete crData;
            //the other code means the erquest is wrongly shaped.
            sendAbort(remoteSource, localDestination, crData->invokedId(), BacnetAbortNS::ReasonOther, true);
            return;
        }

        if (crData->isSegmented()) {
#ifdef NO_SEGMENTATION_SUPPORTED
            qDebug("%s : segmented confrequest received - cannot handle it (%d).", __PRETTY_FUNCTION__, ret);
            sendAbort(remoteSource, localDestination, crData->invokedId(), BacnetAbortNS::ReasonSegmentationNotSupported, true);
            delete crData;
            return;
#else
#error "Not implemented";
            return;
#endif
        }

        _appLayer->processConfirmedRequest(remoteSource, localDestination, data + ret, dataLength - ret, crData);
        break;
    }
    case (BacnetPci::TypeUnconfirmedRequest): {
        /*upon reception: do what's needed and that's all
             */
        BacnetUnconfirmedRequestData ucrData;
        qint32 ret = ucrData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        //! \todo What to send here? If we couldn't parse it we even have no data for reject (invoke id);
        if (ret <= 0) {
            qDebug("%s : Couldn't parse pci data, stop (%d)", __PRETTY_FUNCTION__, ret);
            return;
        }
        _appLayer->processUnconfirmedRequest(remoteSource, localDestination, data + ret, dataLength - ret, ucrData);
        break;
    }
    case (BacnetPci::TypeSimpleAck):
    {
        BacnetSimpleAckData saData;
        qint32 ret = saData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        if (ret <= 0) {
            qDebug("%s : wrong simple ack data (%d)", __PRETTY_FUNCTION__, ret);
            //sendAbort(remoteSource, localDestination, saData.invokedId(), BacnetAbortNS::ReasonOther, true);
            return;
        }

        ExternalConfirmedServiceHandler *service = dequeueConfirmedRequest(remoteSource, localDestination, saData.invokeId());
        Q_CHECK_PTR(service);//this could fail, if there was a timeout for this service. Not an error, just here for the time being.
        if (0 != service)
            _appLayer->processAck(remoteSource, localDestination, data + ret, dataLength - ret, service);
        break;
    }
    case (BacnetPci::TypeComplexAck):
    {
        BacnetComplexAckData cplxData;
        qint32 ret = cplxData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        if (ret <= 0) {
            qDebug("%s : wrong complex ack data (%d)", __PRETTY_FUNCTION__, ret);
            sendAbort(remoteSource, localDestination, cplxData.invokeId(), BacnetAbortNS::ReasonOther, true);
            return;
        }

        if (cplxData.isSegmented()) {
#ifdef NO_SEGMENTATION_SUPPORTED
            qDebug("%s : segmented complex response received - cannot handle it (%d).", __PRETTY_FUNCTION__, ret);
            sendAbort(remoteSource, localDestination, cplxData.invokeId(), BacnetAbortNS::ReasonSegmentationNotSupported, true);
            return;
#else
#error "Not implemented";
            return;
#endif
        }

        ExternalConfirmedServiceHandler *service = dequeueConfirmedRequest(remoteSource, localDestination, cplxData.invokeId());
        Q_CHECK_PTR(service);//this could fail, if there was a timeout for this service. Not an error, just here for the time being.
        if (0 != service)
            _appLayer->processAck(remoteSource, localDestination, data + ret, dataLength - ret, service);
        else
            sendAbort(remoteSource, localDestination, cplxData.invokeId(), BacnetAbortNS::ReasonInvalidApduInThisState, true);
        return;
    }
    case (BacnetPci::TypeSemgmendAck):
    {
        /*upon reception update state machine and send back another segment
             */

#ifdef NO_SEGMENTATION_SUPPORTED
        BacnetSegmentedAckData segData;
        qint32 ret = segData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        if (ret <= 0) {
            qDebug("%s : wrong complex ack data (%d)", __PRETTY_FUNCTION__, ret);
            sendAbort(remoteSource, localDestination, segData.invokeId(), BacnetAbortNS::ReasonOther, true);
        } else {
            qDebug("%s : segment ack rcvd, which is wrong, since we didn;t start the transfer/reception (we don't support it)!", __PRETTY_FUNCTION__);
            sendAbort(remoteSource, localDestination, segData.invokeId(), BacnetAbortNS::ReasonSegmentationNotSupported, true);
        }
        return;
#else
#error "Not implemented";
        return;
#endif

        break;
    }
    case (BacnetPci::TypeError):
    {
        /*BacnetConfirmedRequest seervice failed
             */
        BacnetErrorData errData;
        qint32 ret = errData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        if (ret <= 0) {
            qDebug("%s : wrong error data (%d)", __PRETTY_FUNCTION__, ret);
            return;
        }

        Error error((BacnetServicesNS::BacnetErrorChoice)errData.errorChoice());
        ret = error.appPartFromRaw(data + ret, dataLength - ret);
        if (ret < 0) {
            qDebug("%s : Had problems to analyze error!", __PRETTY_FUNCTION__);
            error.setError(BacnetErrorNS::ClassDevice, BacnetErrorNS::CodeOther);
        }

        ExternalConfirmedServiceHandler *service = dequeueConfirmedRequest(remoteSource, localDestination, errData.invokeId());
        Q_CHECK_PTR(service);//this could fail, if there was a timeout for this service. Not an error, just here for the time being.
        if (0 != service)
            _appLayer->processError(remoteSource, localDestination, error, service);
    }
        break;
    case (BacnetPci::TypeReject):
    {
        /*Protocol error occured
             */
        BacnetRejectData rjctData;
        qint32 ret = rjctData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        if (ret <= 0) {
            qDebug("%s : wrong reject data (%d)", __PRETTY_FUNCTION__, ret);
            return;
        }

        ExternalConfirmedServiceHandler *service = dequeueConfirmedRequest(remoteSource, localDestination, rjctData.invokeId());
        Q_CHECK_PTR(service);//this could fail, if there was a timeout for this service. Not an error, just here for the time being.
        if (0 != service)
            _appLayer->processReject(remoteSource, localDestination, (BacnetRejectNS::RejectReason) rjctData.rejectReason(), service);
    }
        break;
    case (BacnetPci::TypeAbort):
    {
        BacnetAbortData abrtData;
        qint32 ret = abrtData.fromRaw(data, dataLength);
        Q_ASSERT(ret > 0);
        if (ret <= 0) {
            qDebug("%s : wrong abort data (%d)", __PRETTY_FUNCTION__, ret);
            return;
        }

        ExternalConfirmedServiceHandler *service = dequeueConfirmedRequest(remoteSource, localDestination, abrtData.invokeId());
        Q_CHECK_PTR(service);//this could fail, if there was a timeout for this service. Not an error, just here for the time being.
        if (0 != service)
            _appLayer->processAbort(remoteSource, localDestination, service);
    }
        break;
    default: {
        Q_ASSERT(false);
    }
    }
}

void BacnetTSM2::sendAck(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData *data, BacnetConfirmedRequestData *reqData)
{
    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *actualPtr = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    qint32 ret;
    Q_CHECK_PTR(reqData);
    if (0 == data) {//simple ACK
        BacnetSimpleAckData simpleAck(reqData->invokedId(), reqData->service());
        ret = simpleAck.toRaw(actualPtr, buffLength);
        if (ret <= 0) {
            qDebug("BacnetTSM2::sendAck() : Can't write to buff (%d)", ret);
            return;
        }
        buffer.setBodyLength(ret);
        HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "TSM : Sending simple ack message with:");
        _netHandler->sendApdu(&buffer, false,  &destination, &source);
        return;
    }

#ifdef NO_SEGMENTATION_SUPPORTED
    BacnetComplexAckData complexAck(reqData->invokedId(), reqData->service(), 0, 0, false, false);
    ret = complexAck.toRaw(actualPtr, buffLength);
    Q_ASSERT(ret> 0);
    if (ret <= 0) {
        qDebug("BacnetTSM2::sendAck() : Can't write to buff (%d)", ret);
        return;
    }
    actualPtr += ret;
    buffLength -= ret;
    ret = data->toRaw(actualPtr, buffLength);
    Q_ASSERT(ret > 0);
    if (ret < 0) {
        qDebug("BacnetTSM2::sendAck() - can't encode %d", ret);
        return;
    }
    actualPtr += ret;
    buffer.setBodyLength(actualPtr - buffer.bodyPtr());

    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "TSM : Sending ack message with:");
    _netHandler->sendApdu(&buffer, false, &destination, &source);
#else
#error "Not implemented, segmentation!"
#endif
}

void BacnetTSM2::sendAbort(BacnetAddress &remoteDestination, BacnetAddress &localSsource, quint8 invokeId, BacnetAbortNS::AbortReason abortReason, bool fromServer)
{
    BacnetAbortData abort(invokeId, abortReason, fromServer);
    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *actualPtr = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    qint32 ret = abort.toRaw(actualPtr, buffLength);
    Q_ASSERT(ret > 0);
    if (ret < 0) {
        qDebug("%s : can't send abort!", __PRETTY_FUNCTION__);
        return;
    }

    buffer.setBodyLength(ret);
    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "TSM : Sending abort message with:");
    _netHandler->sendApdu(&buffer, false, &remoteDestination, &localSsource);
}

void BacnetTSM2::sendError(BacnetAddress &remoteDestination, BacnetAddress &localSource, quint8 invokeId,
                           BacnetServicesNS::BacnetErrorChoice errorChoice, Error &error)
{
    BacnetErrorData errorData(invokeId, errorChoice);

    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *actualPtr = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    qint32 ret = errorData.toRaw(actualPtr, buffLength);
    Q_ASSERT(ret > 0);
    if (ret < 0) {
        qDebug("BacnetTSM2::sendAck() - can't encode %d", ret);
        Q_ASSERT(false);
        return;
    }
    actualPtr += ret;
    buffLength -= ret;
    ret = error.appPartToRaw(actualPtr, buffLength);
    if (ret < 0) {
        qDebug("%s : Error while reading error.", __PRETTY_FUNCTION__);
        Q_ASSERT(false);
        return;
    }
    actualPtr += ret;

    buffer.setBodyLength(actualPtr - buffer.bodyPtr());
    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "TSM : Sending error message with:");
    _netHandler->sendApdu(&buffer, false, &remoteDestination, &localSource);
}

void BacnetTSM2::sendUnconfirmed(const BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    BacnetUnconfirmedRequestData header(serviceChoice);
    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *actualPtr = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    qint32 ret = header.toRaw(actualPtr, buffLength);
    Q_ASSERT(ret >= 0);
    if (ret < 0) {
        qDebug("BacnetTSM2::sendUnconfirmed() - can't encode %d", ret);
        return;
    }
    buffLength -= ret;
    actualPtr += ret;
    ret = data.toRaw(actualPtr, buffLength);
    Q_ASSERT(ret >= 0);
    if (ret < 0) {
        qDebug("BacnetTSM2::sendUnconfirmed() 2 - can't encode %d", ret);
        return;
    }
    actualPtr += ret;
    buffer.setBodyLength(actualPtr - buffer.bodyPtr());

    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "sendUnconfirmed: ");
    _netHandler->sendApdu(&buffer, false, &destination, &source);
}

Bacnet::BacnetTSM2::ConfirmedRequestEntry::ConfirmedRequestEntry(ExternalConfirmedServiceHandler *handler, int timeout_ms, int retriesNum, const BacnetAddress &destination, const BacnetAddress &source):
    handler(handler),
    timeLeft_ms(timeout_ms),
    retriesLeft(retriesNum),
    dst(destination),
    src(source)
{
}

void Bacnet::BacnetTSM2::timerEvent(QTimerEvent *)
{
    QHash<int, ConfirmedRequestEntry>::Iterator it = _confiremedEntriesList.begin();
    QHash<int, ConfirmedRequestEntry>::Iterator itEnd = _confiremedEntriesList.end();

    while (it != itEnd) {
        it->timeLeft_ms -= _requestTimeout_ms;
        if (it->timeLeft_ms < 0) {
            --(it->retriesLeft);
            if (it->retriesLeft > 0) {//resend
                it->timeLeft_ms = _requestTimeout_ms;
                send_hlpr(it->dst, it->src, it->handler, it.key());
            } else {
                qDebug("%s : Processing service timeout for InvokeId: %d", __PRETTY_FUNCTION__, it.key());
                _appLayer->processTimeout(it->dst, it->src, it->handler);
                _generator.returnId(it.key());
                it = _confiremedEntriesList.erase(it);
                continue;//called to avoid ++it
            }
        }
        ++it;
    }
}

int BacnetTSM2::queueConfirmedRequest(ExternalConfirmedServiceHandler *handler, const BacnetAddress &destination, const BacnetAddress &source)
{
    int invokeId = _generator.generateId();

//#define EXT_COV_TEST
#ifdef EXT_COV_TEST
#warning "CHANGED TEMPORARILY"
    invokeId = 15;
#endif
#undef EXT_COV_TEST

//#define EXT_RP_TEST
#ifdef EXT_RP_TEST
#warning "CHANGED TEMPORARILY"
    invokeId = 1;
#endif
#undef EXT_RP_TEST


    if (invokeId < 0) {
        qDebug("%s : cannot generate id!", __PRETTY_FUNCTION__);
        return invokeId;
    }

    Q_ASSERT(!_confiremedEntriesList.contains(invokeId));
    _confiremedEntriesList.insert(invokeId, ConfirmedRequestEntry(handler, _requestTimeout_ms, _requestRetriesCount, destination, source));
    return invokeId;
}

ExternalConfirmedServiceHandler *BacnetTSM2::dequeueConfirmedRequest(BacnetAddress &remoteSource, BacnetAddress &localDestination, quint8 invokeId)
{
    ExternalConfirmedServiceHandler *handler(0);
    QHash<int, ConfirmedRequestEntry>::Iterator it = _confiremedEntriesList.find(invokeId);
    if (it == _confiremedEntriesList.end()) {
        qDebug("%s : Response for 0x%x requested, and TSM has none.", __PRETTY_FUNCTION__, invokeId);
    } else {
        ConfirmedRequestEntry &entry = it.value();
        if ( (remoteSource == entry.dst) && (localDestination == entry.src) ) {//this is response to our request, indeed.
            handler = entry.handler;
            _confiremedEntriesList.erase(it);
        }
        _generator.returnId(invokeId);
    }

    return handler;
}
