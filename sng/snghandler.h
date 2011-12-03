#ifndef SNGHANDLER_H
#define SNGHANDLER_H

#include <QHostAddress>

class SngHandler
{
public:
    SngHandler();

public:
    void setHostParams(QHostAddress &address, quint64 port);

private:

};

#endif // SNGHANDLER_H
