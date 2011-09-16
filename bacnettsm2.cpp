#include "bacnettsm2.h"

#include "helpercoder.h"
#include "asynchronousbacnettsmaction.h"
#include "bacnetconfirmedservicehandler.h"
#include "bacnetpci.h"

using namespace Bacnet;

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

    _asynchHandlers.append(serviceToSend);

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

    BacnetConfirmedServiceHandler *sH = _asynchHandlers.takeFirst();

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
