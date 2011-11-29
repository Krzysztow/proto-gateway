#ifndef BACNET_RPANSWERER_H
#define BACNET_RPANSWERER_H

#include <QObject>

#include "bacnetaddress.h"
#include "bacnetapplicationlayer.h"

namespace Bacnet {

class RpAnswerer : public QObject
{
    Q_OBJECT
public:
    explicit RpAnswerer(quint8 invokeId, BacnetAddress &answererAddress, BacnetAddress &handlerAddress, BacnetApplicationLayerHandler *appLayerHandler);

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

#endif // BACNET_RPANSWERER_H
