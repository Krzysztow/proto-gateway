#include "covsubscriptionstimehandler.h"

#include <QTimerEvent>

#include "covsupport.h"

using namespace Bacnet;

CovSubscriptionsTimeHandler *CovSubscriptionsTimeHandler::_instance = 0;

CovSubscriptionsTimeHandler::CovSubscriptionsTimeHandler(QObject *parent) :
    QObject(parent),
    _interval_ms(1000)
{
}

CovSubscriptionsTimeHandler *CovSubscriptionsTimeHandler::instance()
{
    if (0 == _instance)
        _instance = new CovSubscriptionsTimeHandler();

    return _instance;
}

void CovSubscriptionsTimeHandler::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    Q_ASSERT(e->timerId() == _timer.timerId());

    // do some meaningful stuff
    QList<CovSupport*>::Iterator it = _covSupporters.begin();
    QList<CovSupport*>::Iterator endIt = _covSupporters.end();

    const int interval_s = _interval_ms/1000;

    /** inform supporters that time has passed. Each returns bool indicating if it still needs timeout notifications (it may no longer).
        need it if, for instance, timeout resulted in deletion of cov subscription due to expiration.
      */
    while (it != endIt) {
        if (!(*it)->timeout(interval_s))
            it = _covSupporters.erase(it);
        else
            ++it;
    }
}

int CovSubscriptionsTimeHandler::interval()
{
    return _interval_ms;
}

void CovSubscriptionsTimeHandler::setInterval(quint8 interval)
{
    if ( _interval_ms != interval) {
        _interval_ms = interval;
        if (_timer.isActive()) {
            _timer.stop();
            _timer.start(_interval_ms, this);
        }
    }
}

void CovSubscriptionsTimeHandler::addCovSupporter(Bacnet::CovSupport *supporter)
{
    if (!_covSupporters.contains(supporter)) {
        _covSupporters.append(supporter);
    }
}

void CovSubscriptionsTimeHandler::rmCovSupporter(Bacnet::CovSupport *supporter)
{
    _covSupporters.removeOne(supporter);
}
