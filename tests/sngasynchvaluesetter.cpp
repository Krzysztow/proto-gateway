#include "sngasynchvaluesetter.h"

#include <QTimer>
#include <QDebug>

#include "property.h"

SngAsynchValueSetter::SngAsynchValueSetter(int interval_ms, QObject *parent) :
    QObject(parent)
{
    if (interval_ms > 0) {
        QTimer *t = new QTimer(this);
        connect(t, SIGNAL(timeout()), this, SLOT(timeout()));
        t->start(interval_ms);
    }
}

void SngAsynchValueSetter::timeout()
{
    if ( (0 == _property) || _valuesToSet.isEmpty())
        return;

    QVariant var = _valuesToSet.takeFirst();

    qDebug()<<"Trying to set property"<<_property->setValue(var);
}

void SngAsynchValueSetter::setProperty(Property *property)
{
    _property = property;
}

void SngAsynchValueSetter::addBalueToSet(QVariant &value)
{
    _valuesToSet.append(value);
}
