#ifndef OBJECTCONNECTIONSUPPORT_H
#define OBJECTCONNECTIONSUPPORT_H

/*
    This file is part of SNGTouchG1.

    SNGTouchG1 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SNGTouchG1 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with SNGTouchG1.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QTime>
#include <QDate>
#include "global.h"
#include "groupaddress.h"

/*!
  @todo Prepare english documentation

  @author Michal Michaluk & Krzysztof Wielgo
  @version 1.0.0
  */

class ConnectionManager;
class ConnectionFrame;
class SNGCONNECTIONMANAGERSHARED_EXPORT ObjectConnectionSupport
{
    Q_DISABLE_COPY(ObjectConnectionSupport);

public:
    enum DimmCommand {StartUp,
                      StopUp,
                      StartDown,
                      StopDown};

public:
    ObjectConnectionSupport();
    virtual ~ObjectConnectionSupport();

protected:
    void registerForAddress(const GroupAddress &address, bool raw=false);
    void unregisterForAddress(const GroupAddress &address, bool raw=false);

protected:
    friend class ConnectionManager;

    void sendOnOff(const GroupAddress &address, bool value, bool excludeMeFromDispose = true);
    void sendDimm(const GroupAddress &address, DimmCommand value, bool excludeMeFromDispose = true);
    void sendTime(const GroupAddress &address, const QTime& value, bool excludeMeFromDispose = true);
    void sendDate(const GroupAddress &address, const QDate& value, bool excludeMeFromDispose = true);
    void sendTemp(const GroupAddress &address, float value, bool excludeMeFromDispose = true);
    void sendValue(const GroupAddress &address, int value, bool excludeMeFromDispose = true);
    void sendFrame(ConnectionFrame &frame, bool excludeMeFromDispose = true);

    virtual void receiveOnOff(const GroupAddress &address, bool value);
    virtual void receiveDimm(const GroupAddress &address, DimmCommand value);
    virtual void receiveTime(const GroupAddress &address, const QTime& value);
    virtual void receiveDate(const GroupAddress &address, const QDate& value);
    virtual void receiveTemp(const GroupAddress &address, float value);
    virtual void receiveValue(const GroupAddress &address, int value);
    virtual void receiveFrame(const ConnectionFrame &frame);

private:

};

#endif // OBJECTCONNECTIONSUPPORT_H
