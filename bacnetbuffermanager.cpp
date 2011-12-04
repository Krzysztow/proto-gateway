#include "bacnetbuffermanager.h"

class BufferWrapper {
public:
    BufferWrapper(quint16 buffSize):
            refCount(0),
            buff(&refCount, buffSize)
    {
        buff._buffer = new quint8[buffSize];
    }

    quint8 refCount;
    Buffer buff;
};

BacnetBufferManager *BacnetBufferManager::_instance = 0;
Buffer BacnetBufferManager::_nullBuff;

BacnetBufferManager::BacnetBufferManager()
{
    _buffersList.reserve(NominalElementsCount);
    for (int i = 0; i < NominalElementsCount; i++) {
        _buffersList.append(new BufferWrapper(MaximumBufferSize));
    }
}

BacnetBufferManager *BacnetBufferManager::instance()
{
    if (0 == _instance) {
        _instance = new BacnetBufferManager();
    }

    return _instance;
}

quint16 BacnetBufferManager::offsetForLayer(RequestingLayer reqLayer)
{
    switch (reqLayer)
    {
    case (ApplicationLayer):
        return OffsetForAPDU;
    case (NetworkLayer):
        return OffsetForNPDU;
    case (TransportLayer):
        //fall through - transport layer is the last one, so we don't need to prepend anything further
    default:
        return 0;
    }
}

Buffer &BacnetBufferManager::getBuffer(RequestingLayer reqLayer)
{
    QList<BufferWrapper*>::iterator it = _buffersList.begin();
    for ( ; it != _buffersList.end(); ++it) {
        if (0 == (*it)->refCount) {

            //fill some informative user values
            Buffer &bRef = (*it)->buff;//we don't invoke copy constructor here, or assignment
            quint16 bodyOffset = offsetForLayer(reqLayer);
            bRef.setBodyPtr(bRef.bufferStart() + bodyOffset);
            bRef.setBodyLength(bRef.buffLength() -  bodyOffset);

            return (*it)->buff;
        }
    }
    //we have run out of free buffers - what to do?
    //! \todo we could increase number of buffers - that's why MaxElementsCount is defined
    //or, (we do it so far), return Buffer instance with empty buffer!
    return _nullBuff;
}
