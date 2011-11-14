#include "bacnettsm2.h"

#include "helpercoder.h"
#include "asynchronousbacnettsmaction.h"
#include "bacnetconfirmedservicehandler.h"
#include "bacnetnetworklayer.h"
#include "bacnetpci.h"
#include "error.h"
#include "bacnetbuffermanager.h"
#include "whoisservicedata.h"

using namespace Bacnet;

BacnetTSM2::InvokeIdGenerator::InvokeIdGenerator()
{
    memset(idsBits, 0, NumberOfOctetsTaken);
}


int BacnetTSM2::InvokeIdGenerator::generateId()
{
    quint8 *bytePtr = idsBits;
    quint8 mask = 0x01;
    quint8 bitNumber;

    for (int i=0; i<NumberOfOctetsTaken; ++i) {
        if (*bytePtr != 0xff) { //there is a free entry
            for (bitNumber = 0; bitNumber < 8; ++bitNumber) {
                if ( (mask & *bytePtr) == 0) { //bit pointing by mask is free
                    *bytePtr |= mask;//reserve field
                    return (8*i + bitNumber);
                }
                mask <<= 1;
            }
        }
        ++bytePtr;
    }
    return -1;
}

void BacnetTSM2::InvokeIdGenerator::returnId(quint8 id)
{
    quint8 byteNumber = id/8;
    quint8 mask = (0x01 << id%8);

    Q_ASSERT( (idsBits[byteNumber] & mask) != 0 );//assert it is used.
    idsBits[byteNumber] &= (~mask);//free bit
}

BacnetTSM2::BacnetTSM2(QObject *parent) :
    QObject(parent)
{
}

void BacnetTSM2::setAddress(InternalAddress &address)
{
    _myRequestAddress = address;
}

InternalAddress &BacnetTSM2::myAddress()
{
    return _myRequestAddress;
}

void BacnetTSM2::discoverDevice(const ObjectIdStruct &deviceId)
{
    BacnetUnconfirmedRequestData reqData(BacnetServices::WhoIs);
    WhoIsServiceData serviceData(objIdToNum(deviceId));

    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *buffStart = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();
    qint32 ret = reqData.toRaw(buffStart, buffLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        qDebug("BacnetTSM2::send() : couldn't write to buffer (pci), %d.", ret);
        return;
    }
    buffStart += ret;
    buffLength -= ret;
    ret = serviceData.toRaw(buffStart, buffLength);
    Q_ASSERT(ret > 0);
    if (ret <= 0) {
        qDebug("BacnetTSM2::send() : couldn't write to buffer (service), %d.", ret);
        return;
    }
    buffStart += ret;
    buffer.setBodyLength(buffStart - buffer.bodyPtr());

    BacnetAddress globalAddr;
    globalAddr.setGlobalBroadcast();

    BacnetAddress srcAddr = BacnetInternalAddressHelper::toBacnetAddress(_myRequestAddress);

    HelperCoder::printArray(buffStart, buffer.bodyLength(), "Discovery request to be sent: ");
    _netHandler->sendApdu(&buffer, false, &globalAddr, &srcAddr);
}

bool BacnetTSM2::deviceAddress(const ObjectIdStruct &deviceId, BacnetAddress *address)
{
    Q_CHECK_PTR(address);
    if (!_routingTable.contains(deviceId))
        return false;

    RoutingEntry &routEntry  = _routingTable[deviceId];
    if (routEntry.isInitialized() && !routEntry.hasExpired()) {
        *address = routEntry.address;
        return true;
    }
    return false;
}

bool BacnetTSM2::send(const BacnetAddress &destination, BacnetAddress &sourceAddress, BacnetServices::BacnetConfirmedServiceChoice service, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms)
{
    //generate invoke id.
    int invokeId = _generator.generateId();
    if (invokeId < 0) {//can't generate
        qDebug("BacnetTSM2::send() : can't generate invoke id. Think about introducing another requesting object.");
        return false;
    }

    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *buffStart = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    BacnetConfirmedRequestData reqData(BacnetConfirmedRequestData::Length_1476Octets, invokeId, service);
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

    HelperCoder::printArray(buffStart, buffer.bodyLength(), "Request to be sent: ");
    qDebug("Length sent %d", ret);

    _netHandler->sendApdu(&buffer, true, &destination, &sourceAddress);

    Q_ASSERT(!_pendingConfirmedRequests.contains(invokeId));
    ConfirmedRequestEntry reqEntry = {serviceToSend, timeout_ms};
    _pendingConfirmedRequests.insert(invokeId, reqEntry);
    //QTimer::singleShot(0, this, SLOT(generateResponse()));
    return true;
}

bool BacnetTSM2::send(const ObjectIdStruct &destinedObject, InternalAddress &sourceAddress, BacnetServices::BacnetConfirmedServiceChoice service, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms)
{
    //find bacnetadderss to send.
    BacnetAddress destAddr;
    if (!deviceAddress(destinedObject, &destAddr)) {
        ConfirmedAwaitingDiscoveryEntry discEntry = {serviceToSend, service, sourceAddress, timeout_ms};
        if (!_awaitingDiscoveryRequests.contains(destinedObject))
            _awaitingDiscoveryRequests.insert(destinedObject, QList<ConfirmedAwaitingDiscoveryEntry>()<<discEntry);
        else
            _awaitingDiscoveryRequests[destinedObject].append(discEntry);
        discoverDevice(destinedObject);
        return true;
    }
    BacnetAddress srcAddr = BacnetInternalAddressHelper::toBacnetAddress(sourceAddress);

    return send(destAddr, srcAddr, service, serviceToSend, timeout_ms);
}


//void BacnetTSM2::generateResponse()
//{
//    BacnetConfirmedServiceHandler::ActionToExecute action;

//    BacnetConfirmedServiceHandler *sH = _pendingConfirmedRequests[0].handler;

//    for (int i = 0; i < 10; ++i) {
//        quint32 t = sH->handleTimeout(&action);
//        if (BacnetConfirmedServiceHandler::ResendService == action) {
//            qDebug("Data resent, next timeout in %d secs.\n", t);

//            if (i == 1) {
//                //quint8 dataRcvd_readAck[] = {0x0c, 0x00, 0x00, 0x00, 0x05, 0x19, 0x55, 0x3e, 0x44, 0x42, 0x90, 0x99, 0x9a, 0x3f};
//                quint8 dataRcvd[] = {};
//                const quint16 dataRcvdLength = sizeof(dataRcvd);
//                HelperCoder::printArray(dataRcvd, dataRcvdLength, "Simulated response rcv'd: ");
//                sH->handleAck(dataRcvd, dataRcvdLength, &action);
//                if (BacnetConfirmedServiceHandler::DeleteServiceHandler == action) {
//                    delete sH;
//                    sH = 0;
//                }
//            }

//        } else {
//            qDebug("Service problem, deleted.");
//            delete sH;
//            sH = 0;
//        }
//    }

//}

void BacnetTSM2::sendReject(BacnetAddress &destination, BacnetAddress &source, BacnetReject::RejectReason reason, quint8 invokeId)
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

    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "Sending reject message with:");
    _netHandler->sendApdu(&buffer, false, &destination, &source);
}

void BacnetTSM2::receive(BacnetAddress &source, BacnetAddress &destination, BacnetSimpleAckData *data)
{
    Q_UNUSED(source);
    Q_UNUSED(destination);
    Q_UNUSED(data);
}

void BacnetTSM2::receive(BacnetAddress &source, BacnetAddress &destination, BacnetComplexAckData *data, quint8 *bodyPtr, quint16 bodyLength)
{
    Q_UNUSED(source);
    Q_UNUSED(destination);
    Q_UNUSED(data);
    Q_UNUSED(bodyPtr);
    Q_UNUSED(bodyLength);
}

void BacnetTSM2::receive(BacnetAddress &source, BacnetAddress &destination, BacnetSegmentedAckData *data, quint8 *bodyPtr, quint16 bodyLength)
{
    Q_UNUSED(source);
    Q_UNUSED(destination);
    Q_UNUSED(data);
    Q_UNUSED(bodyPtr);
    Q_UNUSED(bodyLength);
}

void BacnetTSM2::receive(BacnetAddress &source, BacnetAddress &destination, BacnetErrorData *data, quint8 *bodyPtr, quint16 bodyLength)
{
    Q_UNUSED(source);
    Q_UNUSED(destination);
    Q_UNUSED(data);
    Q_UNUSED(bodyPtr);
    Q_UNUSED(bodyLength);
}

void BacnetTSM2::receive(BacnetAddress &source, BacnetAddress &destination, BacnetRejectData *data, quint8 *bodyPtr, quint16 bodyLength)
{
    Q_UNUSED(source);
    Q_UNUSED(destination);
    Q_UNUSED(data);
    Q_UNUSED(bodyPtr);
    Q_UNUSED(bodyLength);
}

void BacnetTSM2::receive(BacnetAddress &source, BacnetAddress &destination, BacnetAbortData *data, quint8 *bodyPtr, quint16 bodyLength)
{
    Q_UNUSED(source);
    Q_UNUSED(destination);
    Q_UNUSED(data);
    Q_UNUSED(bodyPtr);
    Q_UNUSED(bodyLength);
}

void BacnetTSM2::sendAck(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData *data, quint8 invokeId, quint8 serviceChoice)
{
    //get buffer
    Buffer buffer = BacnetBufferManager::instance()->getBuffer(BacnetBufferManager::ApplicationLayer);
    //write to buffer
    Q_ASSERT(buffer.isValid());
    quint8 *actualPtr = buffer.bodyPtr();
    quint16 buffLength = buffer.bodyLength();

    qint32 ret;
    if (0 == data) {//simple ACK
        BacnetSimpleAckData simpleAck(invokeId, serviceChoice);
        ret = simpleAck.toRaw(actualPtr, buffLength);
        if (ret <= 0) {
            qDebug("BacnetTSM2::sendAck() : Can't write to buff (%d)", ret);
            return;
        }
        buffer.setBodyLength(ret);
        _netHandler->sendApdu(&buffer, false,  &destination, &source);
        HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "Sending simple ack message with:");
        return;
    }

    BacnetComplexAckData complexAck(invokeId, serviceChoice, 0, 0, false, false);
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

    _netHandler->sendApdu(&buffer, false, &destination, &source);
    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "Sending ack message with:");
}

void BacnetTSM2::sendError(BacnetAddress &destination, BacnetAddress &source, quint8 invokeId,
                           BacnetServices::BacnetErrorChoice errorChoice, Error &error)
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
        return;
    }
    actualPtr += ret;
    *actualPtr = error.errorClass;
    ++actualPtr;
    *actualPtr = error.errorCode;
    ++actualPtr;

    buffer.setBodyLength(actualPtr - buffer.bodyPtr());
    _netHandler->sendApdu(&buffer, false, &destination, &source);
    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "Sending error message with:");
}

void BacnetTSM2::sendUnconfirmed(const ObjectIdStruct &destinedObject, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    //find bacnetadderss to send.
    BacnetAddress destAddr;
    if (!deviceAddress(destinedObject, &destAddr)) {
        discoverDevice(destinedObject);
        qDebug("%s : unconfirmed request not sent, since no %d in entry table is preseny. Discovert started.", __PRETTY_FUNCTION__, objIdToNum(destinedObject));
        return;
    }

    return sendUnconfirmed(destAddr, source, data, serviceChoice);
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

    _netHandler->sendApdu(&buffer, false, &destination, &source);
    HelperCoder::printArray(buffer.bodyPtr(), buffer.bodyLength(), "sendUnconfirmed: ");
}
