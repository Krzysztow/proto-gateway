#ifndef BUFFER_H
#define BUFFER_H

#include <QtCore>

class Buffer {
public:
    /**
      Creates empty Buffer (\sa valid() returns false). Used to assign other buffers to it by \sa operator ==
      */
    Buffer();
    ~Buffer() ;

    /**
      Copy constructor of the other Buffer. What it does is increasing reference counter (used by \sa BufferManager
      and point to the same array as the other Buffer instance.
      \note The length and body start are copied, but since now they become idependent of each other!
      */
    Buffer(const Buffer &other);

    /**
      Assignment operator.
      */
    const Buffer &operator=(const Buffer &other);



#ifndef QT_NO_DEBUG
    void seeRefCout() {qDebug("The refCount is %d", (0 != _refCount) ? *_refCount : 0);}
#endif

    /**
      Returns pointer to the first byte of allocated memory block.
      */
    quint8 *bufferStart();

    /**
      Returns number of bytes that are allocated for the buffer.
      */
    quint16 buffLength();

    /**
      Function to set the body length value. This is a helper value.
      \note This value is not shared within the copied/assigned buffers (the buffer memory block is shared indeed)
      */
    void setBodyLength(quint16 length);
    /**
      Function for receiving teh body length set in \sa setBodyLength()
      */
    quint16 bodyLength();

    void setBodyPtr(quint8 *bodyPtr);
    quint8 *bodyPtr();

    /**
      Returns true, if the memory for the buffer is allocated.
      */
    bool isValid() const;

    /**
      Retursn true, if we are NOT the only owner of the buffer.
      */
    bool isShared() const;

    static void printArray(const quint8 *ptr, int size, const char *pretext = "");

private:
    Buffer(quint8 *refCount, quint16 buffLength):
            _refCount(refCount), _buffLength(buffLength){}

    //! \note This is a pointer, not an integer itself, since when we update it, we want to update globally.
    quint8 *_refCount;
    // A pointer to the internal buffer. \note These buffers are assigned internally by \sa DataManager
    quint8 *_buffer;
    // When allocated or copied, value of the length of the buffer is set here.
    quint16 _buffLength;

    /**
      User values - not shared among Buffer instances sharing the same array buffers.
      \note These values are however copied, when assignemnt operator or copy construction is invoked.
      */
    quint16 _bodyLength;
    quint8 *_bodyPtr;
    friend class BufferWrapper;
};

#endif // BUFFER_H
