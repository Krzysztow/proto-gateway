#ifndef GROUPADDRESS_H
#define GROUPADDRESS_H

#include <QtCore>

#include "global.h"

class SNGCONNECTIONMANAGERSHARED_EXPORT GroupAddress
{
public:
    //! Constructs address with provided address parts. If nothing is provided, returns ivalid address.
    GroupAddress(quint8 main = 0, quint8 middle = 0, quint8 sub = 0);

    //! Returns string representing the address.
    QString toString();
    //! Sets address data accordingly to the string provided. Returns false on error.
    bool fromString(QString &strAddr);

    //! Returns true, if not all parts are 0s
    bool isValid() const;
    //! Clears data inside address by setting it to false.
    void setInvalid();

public:
    //! Creates new Group address from string. To chack if all went fine, call \sa isValid() on that address.
    static GroupAddress addrFromString(QString &address);

public:
    inline bool operator==(const GroupAddress &addr2) const {
        return (main == addr2.main && middle == addr2.middle && sub == addr2.sub);
    }

    bool operator<(const GroupAddress &addr2) const;

    inline uint qHash(const GroupAddress &key) {
        return (key.main << 16) + (key.middle << 8) + (key.sub);
    }

public:
    quint8 main;
    quint8 middle;
    quint8 sub;
};

typedef GroupAddress PhysicalAddress;

#endif // GROUPADDRESS_H
