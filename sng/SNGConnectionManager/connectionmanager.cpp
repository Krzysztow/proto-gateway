#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QStringList>

#include "objectconnectionsupport.h"
#include "connectionmanager.h"
#include "connectionframe.h"

const QVector<ObjectConnectionSupport*> *ConnectionManager::Index::operator[] (const GroupAddress &address)
{
    QVector<ObjectConnectionSupport*>* obj = objects(address.main, address.middle, address.sub, false);
    return obj;
}

const QVector<ObjectConnectionSupport*>* ConnectionManager::Index::at(const GroupAddress &address, bool isAllListener)
{
    if (isAllListener) {
        return &allAddressesHash;
    }

    Q_ASSERT(address.isValid());
    QVector<ObjectConnectionSupport*>* obj = objects(address.main, address.middle, address.sub, false);
    return obj;
}

void ConnectionManager::Index::add(const GroupAddress &address, ObjectConnectionSupport* object, bool isAllListener)
{
    if (isAllListener) {
        if (!allAddressesHash.contains(object)) {
            qDebug("ConnectionManager: all addresses receiver added. ATTENTION: If it's registered for some address it wil get it twice!");
            allAddressesHash.append(object);
        }
        return;
    }

    Q_ASSERT(address.isValid());
    QVector<ObjectConnectionSupport*>* obj = objects(address.main, address.middle, address.sub, true);
    if (obj && !obj->contains(object))
        obj->append(object);
}

void ConnectionManager::Index::remove(const GroupAddress &address, ObjectConnectionSupport* object, bool isAllListener)
{
    if (isAllListener) {
        remove(object);
        return;
    }

    Q_ASSERT(address.isValid());
    QVector<ObjectConnectionSupport*>* obj = objects(address.main, address.middle, address.sub, false);
    if (obj)
        obj->remove(obj->indexOf(object));
}

void ConnectionManager::Index::remove(ObjectConnectionSupport* object)
{
    foreach(QVector<QVector<QVector<ObjectConnectionSupport*>*>*>* level0, hash)
    {
        if (level0)
        {
            foreach(QVector<QVector<ObjectConnectionSupport*>*>* level1, *level0)
            {
                if (level1)
                {
                    foreach(QVector<ObjectConnectionSupport*>* level2, *level1)
                    {
                        if (level2)
                        {
                            for(int i=0; i<level2->size();)
                            {
                                if (level2->at(i) == object)
                                {
                                    level2->remove(i);
                                }
                                else ++i;
                            }
                        }
                    }
                }
            }
        }
    }

    int i = allAddressesHash.indexOf(object);
    if (i>-1) {
        qDebug()<<"ConnectionManager: removing object from allAddressesHash";
        allAddressesHash.remove(i);
    }
}

QVector<ObjectConnectionSupport*>* ConnectionManager::Index::objects(
        unsigned int g0,
        unsigned int g1,
        unsigned int g2,
        bool createNew)
{
    QVector<QVector<QVector<ObjectConnectionSupport*>*>*>* level0;

    if ((int) g0 < hash.size())
    {
        level0 = hash.at(g0);
        if (level0 == 0)
        {
            if (createNew)
            {
                hash[g0] = new QVector<QVector<QVector<ObjectConnectionSupport*>*>*>();
                level0 = hash.at(g0);
            } else
                return 0;
        }
    }
    else
    {
        if (createNew)
        {
            hash.resize(g0+1);
            hash[g0] = new QVector<QVector<QVector<ObjectConnectionSupport*>*>*>();
            level0 = hash.at(g0);
        } else
            return 0;
    }

    QVector<QVector<ObjectConnectionSupport*>*>* level1;
    if ((int) g1 < level0->size())
    {
        level1 = (*level0)[g1];
        if (level1 == 0)
        {
            if (createNew)
            {
                (*level0)[g1] = new QVector<QVector<ObjectConnectionSupport*>*>();
                level1 = (*level0)[g1];
            } else
                return 0;

        }
    } else
    {
        if (createNew)
        {
            level0->resize(g1+1);
            (*level0)[g1] = new QVector<QVector<ObjectConnectionSupport*>*>();
            level1 = (*level0)[g1];
        } else
            return 0;
    }
    QVector<ObjectConnectionSupport*>* level2;
    if ((int) g2 < level1->size())
    {
        level2 = (*level1)[g2];
        if (level2 == 0)
        {
            if (createNew)
            {
                (*level1)[g2] = new QVector<ObjectConnectionSupport*>();
                level2 = (*level1)[g2];
            } else
                return 0;
        }
    } else
    {
        if (createNew)
        {
            level1->resize(g2+1);
            (*level1)[g2] = new QVector<ObjectConnectionSupport*>();
            level2 = (*level1)[g2];
        } else
            return 0;
    }
    return level2;
}





ConnectionManager *ConnectionManager::_instance = 0;

ConnectionManager *ConnectionManager::instance()
{
    if (!_instance)
        _instance = new ConnectionManager();
    return _instance;
}

ConnectionManager::ConnectionManager()
{
    _socket = new QTcpSocket(this);
    _ip = "";
    _port = 0;

    connect(_socket, SIGNAL(readyRead()), this, SLOT(readFrame()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
}

void ConnectionManager::sendFrame(ConnectionFrame& frameToSend)
{
    //#define CONNMAN_DEBUG
#ifdef CONNMAN_DEBUG

    switch (frameToSend.dataType())
    {
    case (ConnectionFrame::OnOff):
        qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.onOff();
        break;

    case (ConnectionFrame::Value):
        qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.value();
        break;

    case (ConnectionFrame::Date):
        qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.date();
        break;

    case (ConnectionFrame::Temp):
        qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.temp();
        break;

    case (ConnectionFrame::Time):
        qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.time();
        break;

    case (ConnectionFrame::Dimm):
        qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.dimm();
        break;

    }

#endif

    //set outcoming frame address to ConnectionManager one
    frameToSend.setPhyAddr(_address);
    //    qDebug()<<"Send"<<frameToSend.phyAddress()<<frameToSend.address()<<"type:"<<frameToSend.dataType()<<"value:"<<frameToSend.value();
    _socket->write(frameToSend.data(), frameToSend.sizeOfData());
    disposeFrame(frameToSend);
}

void ConnectionManager::readFrame()
{
    ConnectionFrame frameToRead;
    while (_socket->bytesAvailable() >= frameToRead.sizeOfData())
    {
        _socket->read(frameToRead.data(), frameToRead.sizeOfData());

        if (!frameToRead.isValid()) {
            qDebug("echo invalid frame received");
            return;
        }

        if (isKeepAliveFrame(frameToRead)) {
#ifdef CONNMAN_DEBUG
            qDebug("Keep alive.");
#endif
            return;
        }

#ifdef CONNMAN_DEBUG

        switch (frameToRead.dataType())
        {
        case (ConnectionFrame::OnOff):
            qDebug()<<"Received"<<frameToRead.phyAddress()<<frameToRead.address()<<"type:"<<frameToRead.dataType()<<"value:"<<frameToRead.onOff();
            break;

        case (ConnectionFrame::Value):
            qDebug()<<"Received"<<frameToRead.phyAddress()<<frameToRead.address()<<"type:"<<frameToRead.dataType()<<"value:"<<frameToRead.value();
            break;

        case (ConnectionFrame::Date):
            qDebug()<<"Received"<<frameToRead.phyAddress()<<frameToRead.address()<<"type:"<<frameToRead.dataType()<<"value:"<<frameToRead.date();
            break;

        case (ConnectionFrame::Temp):
            qDebug()<<"Received"<<frameToRead.phyAddress()<<frameToRead.address()<<"type:"<<frameToRead.dataType()<<"value:"<<frameToRead.temp();
            break;

        case (ConnectionFrame::Time):
            qDebug()<<"Received"<<frameToRead.phyAddress()<<frameToRead.address()<<"type:"<<frameToRead.dataType()<<"value:"<<frameToRead.time();
            break;

        case (ConnectionFrame::Dimm):
            qDebug()<<"Received"<<frameToRead.phyAddress()<<frameToRead.address()<<"type:"<<frameToRead.dataType()<<"value:"<<frameToRead.dimm();
            break;
        }

#endif

        disposeFrame(frameToRead);
    }
}

bool ConnectionManager::isKeepAliveFrame(const ConnectionFrame &frame)
{
    //! \note keepAlive frame has fields with following values
    //    frame.paddr0==0x00 && frame.paddr1==0x00 && frame.paddr2==0x00 &&//filter a null frame sent by commserver
    //            frame.gaddr0==0xFF && frame.gaddr1==0xFF && frame.gaddr2==0xFF &&
    //            frame.data_type==0xFF

    static const quint8 keepAliveFrame[] = {/*0xFF, 0xFF, */0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};//we will compare only first bits of dataframe

    //    QString debStr;
    //    QString debStr2;
    const char *ptr=(frame.data()+2);
    //    const quint8 *ptr2=keepAliveFrame;
    //    for (int i=0; i< 7; i++)
    //    {
    //        debStr+=QString::number(*(quint8*)ptr, 16)+" ";
    //        debStr2+=QString::number(*ptr2, 16)+" ";
    //        ptr++;
    //        ptr2++;
    //    }
    //
    //    ptr=(frame.data()+2);
    //    ptr2=keepAliveFrame;

    return ((memcmp(keepAliveFrame, ptr, 7)==0));
}

void ConnectionManager::sendToAll(const ConnectionFrame &frame, const QVector<ObjectConnectionSupport*>* objects)
{
    GroupAddress address(frame.address());

//    const QVector<ObjectConnectionSupport*>* objects = index[address];
    if (objects) {
        foreach(ObjectConnectionSupport* object, *objects)
        {
            switch (frame.dataType())
            {
            case ConnectionFrame::OnOff:
                object->receiveOnOff(address, frame.onOff());
                break;
            case ConnectionFrame::Dimm:
                object->receiveDimm(address, frame.dimm());
                break;
            case ConnectionFrame::Time:
                object->receiveTime(address, frame.time());
                break;
            case ConnectionFrame::Date:
                object->receiveDate(address, frame.date());
                break;
            case ConnectionFrame::Temp:
                object->receiveTemp(address, frame.temp());
                break;
            case ConnectionFrame::Value:
                object->receiveValue(address, frame.value());
                break;
            default:
                break;
            }
        }
    }
}

void ConnectionManager::disposeFrame(const ConnectionFrame &frame)
{
    GroupAddress addr = frame.address();

    //send to objects interensted in particular address
    const QVector<ObjectConnectionSupport*>* objects = connObjects[addr];
    sendToAll(frame, objects);
    //send to objects interested in all addresses
    objects = connObjects.at(addr, true);
    sendToAll(frame, objects);

    //send to raw objects interested in particular adderss
    objects = rawConnObjects[addr];
    if (objects) {
        foreach (ObjectConnectionSupport *obj, *objects) {
            obj->receiveFrame(frame);
        }
    }
    //send to raw objects interested in all frames
    objects = rawConnObjects.at(addr, true);
    foreach (ObjectConnectionSupport *obj, *objects) {
        obj->receiveFrame(frame);
    }
}

void ConnectionManager::error(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "ConnectionManager RemoteHostClosedError";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "ConnectionManager HostNotFoundError";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "ConnectionManager ConnectionRefusedError";
        break;
    default:
        if (_socket->isOpen() && _socket->isValid() && _socket->isReadable() && _socket->isWritable())
        {
            return;
        }
        else
        {
            _socket->close();
            qDebug() << "Unhandled error, reconnectiong";
        }
        break;
    }
    QTimer::singleShot(30*1000, this, SLOT(connectToHost()));
}

void ConnectionManager::connectToHost()
{
    if (_socket->isOpen() && _socket->isValid() && _socket->isReadable() && _socket->isWritable())
    {
        _socket->close();
    }

    qDebug() << "ConnectionManager: connecting to host" << _ip << _port;
    _socket->connectToHost(_ip, _port);
}

void ConnectionManager::configChanged(QDomDocument &config)
{
    QString ip = config.documentElement().attribute("commserver-ip",
                                                    config.documentElement().attribute("router-ip"));

    quint16 port = config.documentElement().attribute("commserver-port",
                                                      config.documentElement().attribute("router-port")).toUShort();


    quint8 main = 1;
    quint8 middle = 0;
    quint8 sub = 1;

    QString phyAddrStr = config.documentElement().attribute("physical-address", "0.0.0");
    QStringList pAParts = phyAddrStr.split(".");
    bool ok = true;
    if (pAParts.size()!=3)
    {
        qDebug()<<"Cannot parse physical-address"<<phyAddrStr;
        ok = false;
    }
    else
    {
        bool ok1= true;
        main = pAParts.at(0).toInt(&ok1);
        ok &=ok1;

        middle = pAParts.at(1).toInt(&ok1);
        ok &=ok1;

        sub = pAParts.at(2).toInt(&ok1);
        ok &=ok1;

        if (!ok) {
            main = 1;
            middle = 0;
            sub = 1;
        }

        qDebug()<<"Changed physical address:"<<main<<middle<<sub;
    }


    if (ip != _ip || port != _port)
    {
        _ip = ip;
        _port = port;
        connectToHost();
    }

#ifdef ENABLE_IP_NOTIFICATION

    if (_notifySocket)
    {
        _notifySocket->close();
        _notifySocket->deleteLater();
        _notifySocket=0;
    }

    if (!_notifyTimer)
        _notifyTimer=new QTimer(this);

    _notifyIp = config.documentElement().attribute("notify-ip", "0.0.0.0");
    _notfiyPort= config.documentElement().attribute("notify-port", "0").toInt();

    if (_notifyIp=="0.0.0.0" || _notfiyPort==0)
    {
        qDebug()<<"Will not connect to any server";
        return;
    }

    ok=false;
    int interval=config.documentElement().attribute("notify-interval").toInt(&ok)*1000*60;
    if (!ok)
        interval=1000*60;//TODO change it to one hour or half an hour

    _notifyMsg=config.documentElement().attribute("name", "Unknown name");

    qDebug()<<"Will connect to server"<<_notifyIp<<_notfiyPort<<"every "<<(interval/1000)<<"seconds";
    connect(_notifyTimer, SIGNAL(timeout()), this, SLOT(sendNotify()), Qt::UniqueConnection);

    _notifyTimer->start(interval);

#endif //ENABLE_IP_NOTIFICATION

}


#ifdef ENABLE_IP_NOTIFICATION
void ConnectionManager::sendNotify()
{
    _notifySocket=new QTcpSocket(this);
    connect(_notifySocket, SIGNAL(destroyed()), this, SLOT(destroyedSocket()));
    _notifySocket->connectToHost(_notifyIp, _notfiyPort);
    if (_notifySocket->waitForConnected(3000))
    {
        qDebug()<<"Connected!";
        QTextStream ts(_notifySocket);
        ts.setAutoDetectUnicode(true);
        ts<<_notifyMsg;
        ts.flush();
        _notifySocket->disconnectFromHost();
        _notifySocket->waitForDisconnected(3000);//if don't manage to disconnect in such time, there is a problem
    }
    else
    {
        qDebug()<<"Couldn't connect to host"<<_notifyIp<<_notfiyPort;
    }
    _notifySocket->deleteLater();
    _notifySocket=0;
}

void ConnectionManager::destroyedSocket()
{
    qDebug()<<"Socket deleted";
}

#endif //ENABLE_IP_NOTIFICATION
