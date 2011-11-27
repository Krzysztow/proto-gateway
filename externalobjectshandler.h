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
                               ExternalPropertyMapping::ReadAccessType type = ExternalPropertyMapping::ReadRP);

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
        bool makeCovSubscription(ExternalPropertyMapping *readElement, bool isConfirmedCovSubscription = false, quint32 lifetime_s = 60000, CovReadStrategy *covStreategy = 0);

    private:
        QHash<Property*, ExternalPropertyMapping*> _mappingTable;

    private:
        QHash<int, ExternalPropertyMapping*> _subscribedCovs;

    private:
        BacnetApplicationLayerHandler *_appLayer;
        QList<InternalAddress> _registeredAddresses;
    };

}

#endif // EXTERNALOBJECTSHANDLER_H
