#include "multipleasynchhelper.h"

#include "cdm.h"

MultipleAsynchHelper::MultipleAsynchHelper()
{
}

int MultipleAsynchHelper::addAsynchIds(QList<int> asynchIds)
{
    //two edge conditionas that are not being rememberered by this class.
    if (asynchIds.count() == 0)
        return ::Property::ResultOk;
    else if (asynchIds.count() == 1)
        return asynchIds.first();

    int genId = DataModel::instance()->generateAsynchId();
    if (genId < 0) {
        qDebug("%s : Cannot generate asynchronous Id.", __PRETTY_FUNCTION__);
        Q_ASSERT(false);
        return ::Property::UnknownError;
    }

    Q_ASSERT(!_asynchIds.contains(genId));
    QHash<int, bool> &newHash = _asynchIds[genId];
    foreach (int aId, asynchIds)
        newHash.insert(aId, false);

    return genId;
}

MultipleAsynchHelper::DoneState MultipleAsynchHelper::asynchIdDone(int asynchId, ::Property::ActiontResult result, int *substituteId, ::Property::ActiontResult *substituteResult)
{
    //bad performance?
    QHash<int, QHash<int, bool> >::Iterator it = _asynchIds.begin();
    QHash<int, QHash<int, bool> >::Iterator itEnd = _asynchIds.end();

    for (; it != itEnd; ++it) {
        if (it->contains(asynchId)) {
            if (::Property::ResultOk != result) {//something wrong happened. That means, the complex asynchId has no chances to be completed with success.

                if (0 != substituteId)
                    *substituteId = it.key();
                if (0 != substituteResult)
                    *substituteResult = result;

                return Finished;
            }

            QHash<int, bool>::Iterator it2 = it->begin();
            QHash<int, bool>::Iterator it2End = it->end();
            bool atLeastOneNotFinished(false);
            for (; it2 != it2End; ++it2) {
                if (it2.key() == asynchId)
                    it2.value() = true;
                else if (it2.value() == false) //current item is not finished
                    atLeastOneNotFinished = true;
            }

            if (atLeastOneNotFinished)//the substituted asynchId action is not finished yet.
                return Accepted;

            //the action consisting of it.values() is finished now. Inform caller and remove its entries.
            if (0 != substituteId)
                *substituteId = it.key();
            if (0 != substituteResult)
                *substituteResult = ::Property::ResultOk;
            _asynchIds.erase(it);
            return Finished;
        }
    }

    //asynchId was not found, we didn't handle it!
    return NotAccepted;
}
