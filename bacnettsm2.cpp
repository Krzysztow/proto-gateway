#include "bacnettsm2.h"

#include "helpercoder.h"
#include "asynchronousbacnettsmaction.h"
#include "bacnetconfirmedservicehandler.h"
#include "bacnetpci.h"
#include "error.h"

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

bool BacnetTSM2::sendAction(BacnetAddress &receiver, AsynchronousBacnetTsmAction *actionToSend)
{
    const quint8 dataSize(64);
    quint8 outData[dataSize];


    qint32 ret = actionToSend->toRaw(outData, dataSize);
    HelperCoder::printArray(outData, ret, "Response to be sent: ");

    //data is sent, deallocate it
    delete actionToSend;
//Q_ASSERT(false);//HOORAY!
}


bool BacnetTSM2::send(ObjectIdStruct &destinedObject, BacnetConfirmedServiceHandler *serviceToSend, quint32 timeout_ms)
{
    //find bacnetadderss to send.


    _pendingConfirmedRequests.insert(0, serviceToSend);

    //generate ivoke id.

    const quint8 dataSize(64);
    quint8 outData[dataSize];


    qint32 ret = serviceToSend->toRaw(outData, dataSize);
    HelperCoder::printArray(outData, ret, "Request to be sent: ");
    qDebug("Length sent %d", ret);

    QTimer::singleShot(0, this, SLOT(generateResponse()));

    //enqueue data
    qDebug("Data enqueued and waits for an ack: timeout in %d\n", timeout_ms);
    return true;
}


void BacnetTSM2::generateResponse()
{
    BacnetConfirmedServiceHandler::ActionToExecute action;

    BacnetConfirmedServiceHandler *sH = _pendingConfirmedRequests[0];

    for (int i = 0; i < 10; ++i) {
        quint32 t = sH->handleTimeout(&action);
        if (BacnetConfirmedServiceHandler::ResendService == action) {
            qDebug("Data resent, next timeout in %d secs.\n", t);

            if (i == 1) {
                //quint8 dataRcvd_readAck[] = {0x0c, 0x00, 0x00, 0x00, 0x05, 0x19, 0x55, 0x3e, 0x44, 0x42, 0x90, 0x99, 0x9a, 0x3f};
                quint8 dataRcvd[] = {};
                const quint16 dataRcvdLength = sizeof(dataRcvd);
                HelperCoder::printArray(dataRcvd, dataRcvdLength, "Simulated response rcv'd: ");
                sH->handleAck(dataRcvd, dataRcvdLength, &action);
                if (BacnetConfirmedServiceHandler::DeleteServiceHandler == action) {
                    delete sH;
                    sH = 0;
                }
            }

        } else {
            qDebug("Service problem, deleted.");
            delete sH;
            sH = 0;
        }
    }

}

void BacnetTSM2::sendReject(BacnetAddress &destination, BacnetAddress &source, BacnetReject::RejectReason reason, quint8 invokeId)
{
    BacnetRejectData rejectData(invokeId, reason);
    quint8 rData[64];
    qint32 ret = rejectData.toRaw(rData, 64);
    Q_ASSERT(ret > 0);
    HelperCoder::printArray(rData, ret, "Sending reject message with:");
}

void BacnetTSM2::sendAck(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData *data, quint8 invokeId, quint8 serviceChoice)
{
    quint8 ackData[64];
    quint8 *actualPtr(ackData);
    quint16 buffLength(sizeof(ackData));
    qint32 ret;
    if (0 == data) {//simple ACK
        BacnetSimpleAckData simpleAck(invokeId, serviceChoice);
        ret = simpleAck.toRaw(ackData, sizeof(ackData));
        HelperCoder::printArray(ackData, ret, "Sending simple ack message with:");
        return;
    }

    BacnetComplexAckData complexAck(invokeId, serviceChoice, 0, 0, false, false);
    ret = complexAck.toRaw(actualPtr, buffLength);
    Q_ASSERT(ret> 0);
    if (ret < 0)
        return;
    actualPtr += ret;
    buffLength -= ret;
    ret = data->toRaw(actualPtr, buffLength);
    Q_ASSERT(ret > 0);
    if (ret < 0) {
        qDebug("BacnetTSM2::sendAck() - can't encode %d", ret);
        return;
    }
    actualPtr += ret;
    HelperCoder::printArray(ackData, actualPtr - ackData, "Sending complex ack message with:");
}

void BacnetTSM2::sendError(BacnetAddress &destination, BacnetAddress &source, quint8 invokeId,
                           BacnetServices::BacnetErrorChoice errorChoice, Error &error)
{
    BacnetErrorData errorData(invokeId, errorChoice);
    quint8 rData[64];
    qint32 ret = errorData.toRaw(rData, 64);
    quint8 *ptr(rData + ret);
    *ptr = error.errorClass;
    ++ptr;
    *ptr = error.errorCode;
    ++ptr;
    Q_ASSERT(ret > 0);
    HelperCoder::printArray(rData, ptr - rData, "Sending error message with:");
}

void BacnetTSM2::sendUnconfirmed(BacnetAddress &destination, BacnetAddress &source, BacnetServiceData &data, quint8 serviceChoice)
{
    BacnetUnconfirmedRequestData header(serviceChoice);
    quint8 rData[64];
    quint8 total(0);
    quint16 length = sizeof(rData);
    qint32 ret = header.toRaw(rData, length);
    Q_ASSERT(ret >= 0);
    if (ret < 0)
        return;
    length -= ret;
    total += ret;
    ret = data.toRaw(rData + ret, length);
    Q_ASSERT(ret >= 0);
    if (ret < 0)
        return;
    total += ret;
    HelperCoder::printArray(rData, total, "sendUnconfirmed: ");
}
