#include "bacnetapplicationlayer.h"

#include "bacnetpci.h"
#include "bacnetcoder.h"

BacnetApplicationLayerHandler::BacnetApplicationLayerHandler(BacnetNetworkLayerHandler *networkHndlr):
        _networkHndlr(networkHndlr)
{
}

BacnetApplicationLayerHandler::~BacnetApplicationLayerHandler()
{
}

void BacnetApplicationLayerHandler::setNetworkHandler(BacnetNetworkLayerHandler *networkHndlr)
{
    _networkHndlr = networkHndlr;
}

void BacnetApplicationLayerHandler::processConfirmedRequest(quint8 *dataPtr, quint16 dataLength)
{
    BacnetConfirmedRequestData serviceData;
    qint16 ret = serviceData.fromRaw(dataPtr, dataLength);
    if (ret >= 0) {
        qDebug("processConfirmedRequest() : incorrect PCI parsing!");
        Q_ASSERT(false);
        return;
    }

    //configure it dynamically
    switch (serviceData.service()) {
    case (BacnetConfirmedService::ConfirmedCOVNotification): {
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    case (BacnetConfirmedService::SubscribeCOV): {
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    case (BacnetConfirmedService::ReadProperty): {
//            BacnetReadProperty readPrpty;
//            readPrpty.setFromRaw()
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    case (BacnetConfirmedService::WriteProperty): {
            Q_ASSERT_X(false, "processConfirmedRequest();", "Unimplemented handler!");
            break;
        }
    default:
        qDebug("processConfirmedRequest() : Unrecognized service. Implement handler!");
        Q_ASSERT(false);
    }

    dataPtr += ret;
    dataLength -= ret;


}

void BacnetApplicationLayerHandler::indication(quint8 *actualBytePtr, quint16 length, BacnetAddress &srcAddr, BacnetAddress &destAddr)
{
    Q_ASSERT(length >= 1);//we need at least first byte for PDU type recognition
    if (length < 1) {
        //send error
        return;
    }

    qint16 ret(0);
    //dispatch to the device!!!


    switch (BacnetPci::pduType(actualBytePtr))
    {
    case (BacnetPci::TypeConfirmedRequest):
        {
            /*upon reception:
              - when no semgenation - do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
              - when segmented - respond with BacnetSegmentAck PDU and when all gotten, do what's needed & send BacnetSimpleAck or BacnetCompletAck PDU
             */
            processConfirmedRequest(actualBytePtr, length);
            break;
        }
    case (BacnetPci::TypeUnconfirmedRequest):
        /*upon reception: do what's needed and that's all
         */
        break;
    case (BacnetPci::TypeSimpleAck):
        /*upon reception update state machine
         */
        break;
    case (BacnetPci::TypeComplexAck):
        /*upon reception update state machine
         */
        break;
    case (BacnetPci::TypeSemgmendAck):
        /*upon reception update state machine and send back another segment
         */
        break;
    case (BacnetPci::TypeError):
        /*BacnetConfirmedRequest seervice failed
         */
        break;
    case (BacnetPci::TypeReject):
        /*Protocol error occured
         */
        break;
    case (BacnetPci::TypeAbort):
        break;
    default: {
            Q_ASSERT(false);
        }
    }








}
