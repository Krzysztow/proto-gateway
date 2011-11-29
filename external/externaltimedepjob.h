#ifndef BACNET_EXTERNALTIMEDEPJOB_H
#define BACNET_EXTERNALTIMEDEPJOB_H

namespace Bacnet {

class ExternalPropertyMapping;
class ExternalObjectsHandler;

class ExternalTimeDepJob
{

public://time-dependant behaviour
    //! returns true, if the action is periodic
    virtual bool isPeriodic() = 0;
    //! returns true, if action is to be executed
    virtual bool timePassed(int timePassed_ms) = 0;
    //! executes action
    virtual void doAction(ExternalPropertyMapping *propertyMapping, ExternalObjectsHandler *externalHandler) = 0;

};

} // namespace Bacnet

#endif // BACNET_EXTERNALTIMEDEPJOB_H
