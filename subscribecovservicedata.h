#ifndef SUBSCRIBECOVSERVICEDATA_H
#define SUBSCRIBECOVSERVICEDATA_H

#include <QtCore>

#include "bacnetservicedata.h"
#include "bacnetcommon.h"
#include "bacnetconstructeddata.h"

/**
   This data is meant to be used both for SubscribeCOV service and SubscribeCOVProperty service. In the first case
   the pointer to BacnetPropertyReference and Increment is simply 0.
  */

namespace Bacnet {

    class SubscribeCOVServiceData:
            public BacnetServiceData
    {
    public:
        SubscribeCOVServiceData();

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:
        inline bool isConfirmedNotificationPresent() {return flags & IssueConfNotifPresent;}
        inline void setConfirmedNotificationPresent() {flags |= IssueConfNotifPresent;}
        inline void clearConfirmedNotificationPresent() {flags &= (~IssueConfNotifPresent);}

        inline bool isLifetimePresent() {return flags & LifetimePresent;}
        inline void setLifetimePresent() {flags |= LifetimePresent;}
        inline void clearLifetimePresent() {flags &= (~LifetimePresent);}

        inline bool hasPropertyReference() {0 != _propReference;}
        inline void clearHasPropertyReference() {delete _propReference; _propReference = 0;}



    public://is there any reason we should make it private?
        quint32 _subscriberProcId;
        ObjectIdStruct _monitoredObjectId;
        bool _issueConfNotification;
        quint32 _lifetime;
        PropertyReference *_propReference;
        BacnetDataInterface *_covIncrement;

    private:
        enum {
            IssueConfNotifPresent = 0x01,
            LifetimePresent     = 0x02,

            AllFlags = IssueConfNotifPresent | LifetimePresent
        };
        quint8 _flags;
    };

}
#endif // SUBSCRIBECOVSERVICEDATA_H
