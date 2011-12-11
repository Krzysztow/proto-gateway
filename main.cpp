#include <QtCore/QCoreApplication>
#include <QDomDocument>

#define MAIN_BACNET
#ifdef MAIN_BACNET


#include "cdm.h"
#include "protocolsmanager.h"

using namespace GatewayApplicationNS;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ProtocolsManager protoManager("gateway-main-cfg.xml");
    DataModel::instance()->startFactory();  //it contains Property instances, so we start DataModel
    protoManager.init();
    DataModel::instance()->stopFactory();

    return a.exec();
}
#endif
