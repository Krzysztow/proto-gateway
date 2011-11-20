#ifndef MULTIPLEASYNCHHELPER_H
#define MULTIPLEASYNCHHELPER_H

#include <QHash>
#include "property.h"

class MultipleAsynchHelper
{
public:
    MultipleAsynchHelper();

    //! Expects list containing asynchronous id's that were are to be guarded. Returns new asynch Id, that will indicate all the id's are finished.
    int addAsynchIds(QList<int> asynchIds);

    enum DoneState {
        NotAccepted,
        Accepted,
        Finished
    };

    /** Expects asynchId of action and it's result. If contains such asynchId, returns Accepted or Finished, so that caller don't forward it any further.
      In case of Accepted, caller does nothing more. When Finished is returned, it sets also substituteId and substituteResult pointers which indicate that
      complex action (consisting of few asynchIds provided by \sa addAsynchIds()) is finished with result substituteResult.
      */
    DoneState asynchIdDone(int asynchId, ::Property::ActiontResult result, int *substituteId, ::Property::ActiontResult *substituteResult);

private:
    QHash<int, QHash<int, bool> > _asynchIds;
};

#endif // MULTIPLEASYNCHHELPER_H
