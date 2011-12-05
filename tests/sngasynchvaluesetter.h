#ifndef SNGASYNCHVALUESETTER_H
#define SNGASYNCHVALUESETTER_H

#include <QObject>
#include <QList>
#include <QVariant>

class Property;

class SngAsynchValueSetter : public QObject
{
    Q_OBJECT
public:
    explicit SngAsynchValueSetter(int interval_ms, QObject *parent = 0);

    void addBalueToSet(QVariant &value);

signals:

public slots:
    void timeout();
    void setProperty(Property *property);

private:
    QList<QVariant> _valuesToSet;

    Property *_property;

};

#endif // SNGASYNCHVALUESETTER_H
