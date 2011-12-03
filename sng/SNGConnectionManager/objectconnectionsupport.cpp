#include "objectconnectionsupport.h"

#include "connectionmanager.h"
#include "connectionframe.h"

class ObjectConnectionSupportPrivate
{
};

ObjectConnectionSupport::ObjectConnectionSupport()
{
}

ObjectConnectionSupport::~ObjectConnectionSupport()
{
    ConnectionManager::instance()->connObjects.remove(this);
    ConnectionManager::instance()->rawConnObjects.remove(this);
}

void ObjectConnectionSupport::registerForAddress(const GroupAddress &address, bool raw)
{
    if (!address.isValid()) {
        qDebug() << "ObjectConnectionSupport:" << "registring for null group address";
    }

    if (!raw) {
        ConnectionManager::instance()->connObjects.add(address, this);
    }
    else {
        ConnectionManager::instance()->rawConnObjects.add(address, this);
    }
}

void ObjectConnectionSupport::unregisterForAddress(const GroupAddress &address, bool raw)
{
    if (!raw) {
        ConnectionManager::instance()->connObjects.remove(address, this);
    } else {
        ConnectionManager::instance()->rawConnObjects.remove(address, this);
    }
}

void ObjectConnectionSupport::sendOnOff(const GroupAddress &address, bool value)
{
    if (address.isValid()) {
        ConnectionFrame frameToSend;
        frameToSend.setGrAddress(address);
        frameToSend.setOnOff(value);
        ConnectionManager::instance()->sendFrame(frameToSend);
    }
}

void ObjectConnectionSupport::sendDimm(const GroupAddress &address, DimmCommand value)
{
    if (address.isValid())
    {
        ConnectionFrame frameToSend;
        frameToSend.setGrAddress(address);
        frameToSend.setDimm(value);
        ConnectionManager::instance()->sendFrame(frameToSend);
    }
}

void ObjectConnectionSupport::sendTime(const GroupAddress &address, const QTime& value)
{
    if (address.isValid())
    {
        ConnectionFrame frameToSend;
        frameToSend.setGrAddress(address);
        frameToSend.setTime(value);
        ConnectionManager::instance()->sendFrame(frameToSend);
    }
}

void ObjectConnectionSupport::sendDate(const GroupAddress &address, const QDate& value)
{
    if (address.isValid())
    {
        ConnectionFrame frameToSend;
        frameToSend.setGrAddress(address);
        frameToSend.setDate(value);
        ConnectionManager::instance()->sendFrame(frameToSend);
    }
}

void ObjectConnectionSupport::sendTemp(const GroupAddress &address, float value)
{
    if (address.isValid())
    {
        ConnectionFrame frameToSend;
        frameToSend.setGrAddress(address);
        frameToSend.setTemp(value);
        ConnectionManager::instance()->sendFrame(frameToSend);
    }
}

void ObjectConnectionSupport::sendValue(const GroupAddress &address, int value)
{
    if (address.isValid())
    {
        ConnectionFrame frameToSend;
        frameToSend.setGrAddress(address);
        frameToSend.setValue(value);
        ConnectionManager::instance()->sendFrame(frameToSend);
    }
}

void ObjectConnectionSupport::sendFrame(ConnectionFrame &frame)
{
    qDebug()<<"Sends frame with group addr:"<<frame.address().toString()<<"phyAddr"<<frame.phyAddress().toString()<<"of type"<<frame.dataType()<<"value"<<frame.value();
    if (frame.isValid())
        ConnectionManager::instance()->sendFrame(frame);
}

void ObjectConnectionSupport::receiveOnOff(const GroupAddress &address, bool value)
{ Q_UNUSED(address); Q_UNUSED(value); }
void ObjectConnectionSupport::receiveDimm(const GroupAddress &address, DimmCommand value)
{ Q_UNUSED(address); Q_UNUSED(value); }
void ObjectConnectionSupport::receiveTime(const GroupAddress &address, const QTime& value)
{ Q_UNUSED(address); Q_UNUSED(value); }
void ObjectConnectionSupport::receiveDate(const GroupAddress &address, const QDate& value)
{ Q_UNUSED(address); Q_UNUSED(value); }
void ObjectConnectionSupport::receiveTemp(const GroupAddress &address, float value)
{ Q_UNUSED(address); Q_UNUSED(value); }
void ObjectConnectionSupport::receiveValue(const GroupAddress &address, int value)
{ Q_UNUSED(address); Q_UNUSED(value); }
void ObjectConnectionSupport::receiveFrame(const ConnectionFrame &frame)
{ Q_UNUSED(frame); }
