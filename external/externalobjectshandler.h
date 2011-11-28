#ifndef EXTERNALOBJECTSHANDLER_H
#define EXTERNALOBJECTSHANDLER_H

#include <QtCore>
#include <QObject>

#include "propertyowner.h"
#include "property.h"
#include "externalpropertymapping.h"
#include "bacnetcommon.h"
#include "bacnetinternaladdresshelper.h"


namespace Bacnet {

    class BacnetReadPropertyAck;
    class BacnetTSM2;
    class ExternalConfirmedServiceHandler;
    class Error;
    class BacnetApplicationLayerHandler;
    class CovReadStrategy;
    class ExternalObjectReadStrategy;

    class ExternalObjectsHandler:
            public QObject,
            public PropertyOwner
    {
        Q_OBJECT
    public:
        ExternalObjectsHandler(BacnetApplicationLayerHandler *appLayer);
        ~ExternalObjectsHandler();

        void addMappedProperty(PropertySubject *property, ObjIdNum objectId,
                               BacnetPropertyNS::Identifier propertyId, quint32 propertyArrayIdx,                               
                               ExternalObjectReadStrategy *readStrategy = 0);

        ExternalPropertyMapping *mappingEntry(::Property *property);

        void handleResponse(ExternalConfirmedServiceHandler *act, BacnetReadPropertyAck &rp);
        void handleResponse(ExternalConfirmedServiceHandler *act, bool ok);//all the simple acks come here.
        void handleError(ExternalConfirmedServiceHandler *act, Error &error);
        void handleAbort(ExternalConfirmedServiceHandler *act,  quint8 abortReason);

        bool isRegisteredAddress(InternalAddress &address);
        void addRegisteredAddress(InternalAddress &address);
        void removeRegisteredAddress(InternalAddress &address);
        BacnetAddress oneOfAddresses();

    public://overridden from PropertyOwner
        int getPropertyRequested(PropertySubject *toBeGotten);
        virtual int setPropertyRequested(PropertySubject *toBeSet, QVariant &value);

        /** This function shouldn't be ever called. This owner should contain only external objects, which are
          handled by themselves.
          */
        virtual void asynchActionFinished(int asynchId, Property *property, Property::ActiontResult actionResult);

        /** This function is not used, since all the objects here are PropertySubjects and will not be changed
          from within CDM.
         */
        virtual void propertyValueChanged(Property *property);

    public:
        int readProperty(ExternalPropertyMapping *readElement, bool askStrategy = true);
    private:
        QHash<Property*, ExternalPropertyMapping*> _mappingTable;

    protected:
        void timerEvent(QTimerEvent *);
    private:
        typedef QPair<ExternalObjectReadStrategy*,ExternalPropertyMapping*> TTimeDependantPair;
        QList<TTimeDependantPair> _timeDependantJobs;
        QBasicTimer _timer;
        static const int DefaultInterval_ms = 500;
        int _interval_ms;

    public:
        bool startCovSubscriptionProcess(ExternalPropertyMapping *propertyMapping, bool isConfirmedCovSubscription = false, quint32 lifetime_s = 60000, CovReadStrategy *covStreategy = 0);
        void subscriptionProcessFinished(int subscribeProcId, ExternalPropertyMapping *propertyMapping, CovReadStrategy *readStrategy, bool ok, bool isCritical = false);
    private:
        /**
          The hash works as single key one, but 0 key. For zero it may have multiple values, which mean unconfirmed notifications.
          */
        typedef QPair<ExternalPropertyMapping*, CovReadStrategy*> TCovMappinPair;
        QHash<int, TCovMappinPair> _subscribedCovs;
        static const int UnconfirmedProcIdValue = 0;
        static const quint32 MaximumConfirmedSubscriptions = 255;
        int _lastProcIdValueUsed;
        int insertToOrFindSubscribeCovs(bool confirmed, ExternalPropertyMapping *propertyMapping, CovReadStrategy *readStrategy, int valueToCheck = -1);

    private:
        BacnetApplicationLayerHandler *_appLayer;
        QList<InternalAddress> _registeredAddresses;
    };

}

#endif // EXTERNALOBJECTSHANDLER_H
