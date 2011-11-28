#ifndef SUBSCRIBECOVSERVICEDATA_H
#define SUBSCRIBECOVSERVICEDATA_H

#include <QtCore>

#include "bacnetservicedata.h"
#include "bacnetcommon.h"
#include "bacnetconstructeddata.h"
#include "covincrementhandlers.h"

/**
   This data is meant to be used both for SubscribeCOV service and SubscribeCOVProperty service. In the first case
   the pointer to BacnetPropertyReference and Increment is simply 0.
   NOTE: According to standard, when IssueconfirmedNotifications and Lifetime are absent, then this is a cancellation request.
  */

namespace Bacnet {

    class SubscribeCOVServiceData:
            public BacnetServiceData
    {
    public:
        //! Sets default value. Would be used for parsing.
        SubscribeCOVServiceData();
        //! Prepares data as unsubscription.
        SubscribeCOVServiceData(quint32 subscriberProcessId, ObjIdNum monitoredObjectId);
        //! Creates data as -/re-subscription.
        SubscribeCOVServiceData(quint32 subscriberProcessId, ObjIdNum monitoredObjectId, bool issueConfirmedNotifications, bool hasLifetime, quint32 lifetime = IndefiniteLifetime,
                                BacnetPropertyNS::Identifier propId = BacnetPropertyNS::UndefinedProperty, quint32 propIdx = ArrayIndexNotPresent);
        ~SubscribeCOVServiceData();

        static const quint32 IndefiniteLifetime = 0;

    public://implementations of BacnetServiceData interface.
        virtual qint32 toRaw(quint8 *startPtr, quint16 buffLength);
        virtual qint32 fromRaw(quint8 *serviceData, quint16 buffLength);

    public:        
        inline bool isConfirmedNotificationPresent() {return _flags & IssueConfNotifPresent;}
        inline void setConfirmedNotificationPresent() {_flags |= IssueConfNotifPresent;}
        inline void clearConfirmedNotificationPresent() {_flags &= (~IssueConfNotifPresent);}

        inline bool isLifetimePresent() {return _flags & LifetimePresent;}
        inline void setLifetimePresent() {_flags |= LifetimePresent;}
        inline void clearLifetimePresent() {_flags &= (~LifetimePresent);}

        void setIsCancellation();
        bool isCancellation();

        inline bool hasPropertyReference() {return 0 != _propReference;}
        inline void clearHasPropertyReference() {delete _propReference; _propReference = 0;}
        inline PropertyReference *takePropReference() {PropertyReference *tmp = _propReference; _propReference = 0; return tmp;}

        inline bool hasCovIncrement() {return 0 != _covIncrement;}
        inline void clearCovIncrement() {delete _covIncrement; _covIncrement = 0;}
        inline CovRealIcnrementHandler *takeCovIncrement() {CovRealIcnrementHandler *tmp = _covIncrement; _covIncrement = 0; return tmp;}
        void setCovIncrement(float value);

    public://is there any reason we should make it private?
        quint32 _subscriberProcId;
        ObjectIdentifier _monitoredObjectId;
        bool _issueConfNotification;
        quint32 _lifetime;
        PropertyReference *_propReference;
        CovRealIcnrementHandler *_covIncrement;

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
