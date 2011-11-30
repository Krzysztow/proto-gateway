#ifndef CDM_H
#define CDM_H

#include <QDomElement>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QObject>
#include <limits>

class Property;
class PropertyOwner;
class PropertySubject;
class PropertyObserver;
class DataModel:
    public QObject
{
    Q_OBJECT;
public:
    static DataModel *instance();

    /**Creates PropertySubject of propertyType and registers it at propId id. If the id is already reserverd
      returns 0 pointer.
      */
    PropertySubject *createPropertySubject(quint32 propId, QVariant::Type propertyType);

    Property *createProperty(QDomElement &propElement);
    PropertySubject *createPropertySubject(QDomElement &subjectElement);
    PropertyObserver *createPropertyObserver(QDomElement &observerElement);

    PropertySubject *getProperty(quint32 propId);

    PropertyObserver *createPropertyObserver(quint32 propId);

    //! \todo inside the code, there is a shift between asynchId and index (returned 0 is ResultOK code) - fix it
    /** This function should be used to get an unique id for the asynchronous operation within DataModel.
      \param owner - pointer to the PropertyOwner instance, which is an owner of a property for which
                    asynchronous operation is being invoked. This owner will be called in case of internal timeout.
      \returns - asynchronous id. If value is negative, this means there are no free ids anymore. It's not likely
                    to happen, but if so:
                    - check if all of the PropertyOwner instances return asynchronous operations id in time (probably some don't)
                    - the traffic of the gateway is really hard (optimize!),
                    - or increase MAX_ASYNCH_ID value;
        if negative is returned that means there are no more available asynch ids.
      */
    int generateAsynchId();
    void setAsynchIdData(int asynchId, PropertySubject *subject, PropertyObserver *requester = 0);
    //Returns the asynchronous id to the DataModel instance. No one (owner or requester) is informed.
    void releaseAsynchId(int id);
    PropertyObserver *asynchActionRequester(int asynchId);
    PropertySubject *asynchActionSubject(int asynchId);

protected:
    void timerEvent(QTimerEvent *);

private:
    DataModel(QObject *parent = 0);
    virtual ~DataModel();

    void initiateAsynchIds();

private:
    static DataModel *_instance;

    //value given in 1/10 of second. Default timeout is 1 second.
    static const int DEFAULT_TIMEOUT = 2000;
    int _internalTimeout_ms;

    QMap<quint32, PropertySubject*> _properties;
    /** This is an array saying which ids are currently in use. Its size is as great as is necessarry to store MAX_ASYNCH_ID bits.
        Same the maximum number of parallely occuring asynchronous actions is MAX_ASYNCH_ID. When changing this value,
        make sure that int type returned by \sa generateAsynchId() and used in clearAsynchId() are enough to store it.
      */
    static const int MAX_ASYNCH_ID = 255;

    /** The data model has to take care of stale transactions. If not, then some transactions may be never released. This
        This list is created so that, the model checks if any transaction is not too old. If so, it calls
        PropertyOwner::asynchOpertationDone() with TIMEOUT parameter and clears internal data used for
        asynchId (_asynchIds bit and _asynchIdInfo entry).
      */
    struct AsynchIdEntry {
        int timeLeft;
        PropertySubject *subjectProperty;
        PropertyObserver *requestingObserver;
    };
    QVector<AsynchIdEntry> _asynchIdStates;

    static const int UNUSED_TIME_VALUE;
    inline bool isAsynchIdUnused(int asynchId) {return (UNUSED_TIME_VALUE == _asynchIdStates[asynchId].timeLeft);}
    inline void setAsynchIdUnused(int asynchId) {_asynchIdStates[asynchId].timeLeft = UNUSED_TIME_VALUE;}
};

#endif // CDM_H
