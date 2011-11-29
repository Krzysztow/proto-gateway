#ifndef BACNET_EXTERNALOBJECTWRITESTRATEGY_H
#define BACNET_EXTERNALOBJECTWRITESTRATEGY_H

#include <QVariant>
#include "externaltimedepjob.h"

namespace Bacnet {

class ExternalPropertyMapping;
class ExternalObjectsHandler;


class ExternalObjectWriteStrategy:
        public ExternalTimeDepJob
{
public:
    virtual int writeProperty(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler, QVariant &valueToWrite, bool generateAsynchId = false);

public://time-dependant behaviour
    //! returns true, if the action is periodic
    virtual bool isPeriodic();
    //! returns true, if action is to be executed
    virtual bool timePassed(int timePassed_ms);
    //! executes action
    virtual void doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler);
};

} // namespace Bacnet

#endif // BACNET_EXTERNALOBJECTWRITESTRATEGY_H
