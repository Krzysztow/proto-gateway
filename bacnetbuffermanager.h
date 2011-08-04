#ifndef BACNETBUFFERMANAGER_H
#define BACNETBUFFERMANAGER_H

#include <QList>

#include "bacnetcommon.h"
#include "buffer.h"

class BufferWrapper;
class BacnetBufferManager
{
public:
    enum {
        NominalElementsCount = 4,
        MaxElementsCount = NominalElementsCount,
    };

    enum MemoryManagerConsts {

        /**
          Memory manager allocates constant size frames - they are of the maximum size that can be
          received/sent via the lowest layer. When receive, we don't have to take care of bytes placement.
          However, when sending data, we have to know at what place to insert e.g. application data,
          so that all the information from lower layers have place to be inserted as well. We could put
          them at the beginnging and move by the lower layers, but this is time-consuming. Here we choose
          worst-case scenario (each lower layer takes would taken maximum bytes it can) and start placing upper
          layer data at the offset, resulting from taking into consideration space that lower layers could
          fill - this is not so memory efficient, but well performing and still no memory fragmentation and
          allocation is needed afterwards.
          */

        MaximumBufferSize = BacnetCommon::BvllMaxSize,
        OffsetForNPDU = (MaximumBufferSize - BacnetCommon::NpduMaxSize),
        OffsetForAPDU = (OffsetForNPDU + BacnetCommon::NpduMaxHeaderSize)
    };

    enum RequestingLayer {
        ApplicationLayer,
        NetworkLayer,
        TransportLayer
    };

    static BacnetBufferManager *instance();

    /**
      This function returns the vacant Buffer instance from the pool of Buffers.
      \note The Buffer::bodyLength()
      \warning When there is no more vacant isntances, this function returns an empty buffer which is invalid.
      It's a user responsibility to check for validitidy - \sa Buffer::isValid()
      */
    Buffer &getBuffer(RequestingLayer reqLayer);

    /**
      Returns nominal offsets for each layer. For instance, when we want to get offset for APDU in the buffer,
      call offsetForLayer(ApplicationLayer).
      */
    quint16 offsetForLayer(RequestingLayer reqLayer);
private:

private:
    // Prevent others from creating an instance - SingletonPattern
    BacnetBufferManager();

    static BacnetBufferManager *_instance;

    QList<BufferWrapper*> _buffersList;
    static Buffer _nullBuff;
};

#endif // BACNETBUFFERMANAGER_H
