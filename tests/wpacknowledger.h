#ifndef BACNET_WPACKNOWLEDGER_H
#define BACNET_WPACKNOWLEDGER_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetapplicationlayer.h"

namespace Bacnet {

class WpAcknowledger : public QObject
{
    Q_OBJECT
public:
    explicit WpAcknowledger(quint8 invokeId, BacnetAddress &answererAddress, BacnetAddress &handlerAddress, BacnetApplicationLayerHandler *appLayerHandler);

signals:

public slots:
    void answer();

private:
    quint8 invokeId;
    BacnetAddress &answererAddress;
    BacnetAddress &handlerAddress;
    BacnetApplicationLayerHandler *appLayerHandler;
};

} // namespace Bacnet

#endif // BACNET_WPACKNOWLEDGER_H
