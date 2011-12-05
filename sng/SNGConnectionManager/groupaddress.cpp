#include "groupaddress.h"

#include <stdio.h>

GroupAddress::GroupAddress(quint8 main, quint8 middle, quint8 sub):
    main(main),
    middle(middle),
    sub(sub)
{    
}

bool GroupAddress::fromString(QString &strAddr)
{
    unsigned int g2, g1, g0;
    if (sscanf(strAddr.toAscii().data(), "%u/%u/%u",
               &g0,
               &g1,
               &g2) == 3)
    {
        main        = (quint8) g0;
        middle      = (quint8) g1;
        sub         = (quint8) g2;
        return true;
    }
    return false;
}

QString GroupAddress::toString()
{
    return QString("%1/%2/%3").arg(main).arg(middle).arg(sub);
}

GroupAddress GroupAddress::addrFromString(QString &address)
{
    GroupAddress grAddr;
    grAddr.fromString(address);
    return grAddr;
}

bool GroupAddress::isValid() const
{
    return ( (main != 0) ||
             (middle != 0) ||
             (sub != 0) );
}

void GroupAddress::setInvalid()
{
    main=0; middle=0; sub=0;
}

bool GroupAddress::operator <(const GroupAddress &addr2) const
{
    if (main < addr2.main)
        return true;
    else if (main > addr2.main)
        return false;
    if (middle < addr2.middle)
        return true;
    if (middle > addr2.middle)
        return false;
    if (sub < addr2.sub)
        return true;
    return false;
}
