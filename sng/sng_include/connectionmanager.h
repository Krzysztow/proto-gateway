#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QDomDocument>
#include "global.h"

#ifdef ENABLE_IP_NOTIFICATION
#include <QTimer>
#endif //ENABLE_IP_NOTIFICATION

#include "connectionframe.h"
//class ConnectionFrame;
class ObjectConnectionSupport;

class SNGCONNECTIONMANAGERSHARED_EXPORT ConnectionManager : public QObject
{
    Q_OBJECT;
    Q_DISABLE_COPY(ConnectionManager);

public:
    static ConnectionManager *instance();

private:
    ConnectionManager();

public slots:
    void sendFrame(ConnectionFrame &frameToSend);
    void configChanged(QDomDocument &config);

private slots:
    void connectToHost();
    void readFrame();
    void error(QAbstractSocket::SocketError socketError);

private:
    void disposeFrame(const ConnectionFrame &frame);
    void sendToAll(const ConnectionFrame &frame, const QVector<ObjectConnectionSupport*>* objects);
    bool isKeepAliveFrame(const ConnectionFrame &frame);

private:
    static ConnectionManager *_instance;
    GroupAddress _address;

private:
    QTcpSocket *_socket;
    QString _ip;
    quint16 _port;

#ifdef ENABLE_IP_NOTIFICATION
private:
    QTcpSocket *_notifySocket;
    QTimer *_notifyTimer;

    QString _notifyIp;
    QString _notifyMsg;
    quint16 _notfiyPort;

private slots:
    void sendNotify();
    void notifySocketDisconnected();
    void destroyedSocket();
#endif //ENABLE_IP_NOTIFICATION

private:
    class Index
    {
    public:
        const QVector<ObjectConnectionSupport*> *operator[] (const GroupAddress &address);
        const QVector<ObjectConnectionSupport*> *at(const GroupAddress &address, bool isAllListener = false);
        void add(const GroupAddress &address, ObjectConnectionSupport *object, bool isAllListener = false);
       //! \warning \todo if the OCS is registered for all addresses this call will not unregister anytinhng. Then it should be called with "*/*/*" address.
        void remove(const GroupAddress &address, ObjectConnectionSupport *object, bool isAllListener = false);
        //! \note this call will unregister OCS from particular addresses and from all addresses
        void remove(ObjectConnectionSupport* object);

    private:
        QVector<QVector<QVector<QVector<ObjectConnectionSupport*>*>*>*> hash;
        QVector<ObjectConnectionSupport*> allAddressesHash;

        QVector<ObjectConnectionSupport*> *objects(unsigned int g0, unsigned int g1, unsigned int g2, bool createNew);

    };

public:
    Index connObjects;
    Index rawConnObjects;
};


#endif // CONNECTIONMANAGER_H
