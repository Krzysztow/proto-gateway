#ifndef COVSUBSCRIPTIONSTIMEHANDLER_H
#define COVSUBSCRIPTIONSTIMEHANDLER_H

#include <QObject>
#include <QBasicTimer>

namespace Bacnet {

class CovSupport;

class CovSubscriptionsTimeHandler : public QObject
{
    Q_OBJECT
public:
    static CovSubscriptionsTimeHandler *instance();
    void setInterval(quint8);
    int interval();

    void addCovSupporter(CovSupport *supporter);
    void rmCovSupporter(CovSupport *supporter);

protected:
    void timerEvent(QTimerEvent *e);

signals:

public slots:

private:
    CovSubscriptionsTimeHandler(QObject *parent = 0);
    static CovSubscriptionsTimeHandler *_instance;

    QBasicTimer _timer;
    int _interval_ms;

    QList<CovSupport*> _covSupporters;
};

}

#endif // COVSUBSCRIPTIONSTIMEHANDLER_H
