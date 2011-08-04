#include "buffer.h"
#include <stdio.h>

Buffer::Buffer():
        _refCount(0),
        _buffer(0),
        _buffLength(0),
        _bodyLength(0),
        _bodyPtr(0)
{}

Buffer::~Buffer() {
    if (0 != _refCount) {
        --(*_refCount);
    }
}

Buffer::Buffer(const Buffer &other):
        _refCount(other._refCount),
        _buffer(other._buffer),
        _buffLength(other._buffLength),
        _bodyLength(other._bodyLength),
        _bodyPtr(other._bodyPtr)
{
    if (0 != _refCount)
        ++(*_refCount);
}

const Buffer &Buffer::operator=(const Buffer &other) {
    if (_buffer == other._buffer)//we are the same
        return *this;
    else {
        if (0 != _refCount)
            --(*_refCount);

        _buffer = other._buffer;
        _buffLength = other._buffLength;
        _bodyLength = other._bodyLength;
        _refCount = other._refCount;
        _bodyPtr = other._bodyPtr;

        if (0 != other._refCount)
            ++(*other._refCount);
        return other;
    }
}

quint8 *Buffer::bufferStart()
{
    return _buffer;
}

quint16 Buffer::buffLength()
{
    return _buffLength;
}

void Buffer::setBodyLength(quint16 length) {
    _bodyLength = length;
}

quint16 Buffer::bodyLength() {
    return _bodyLength;
}

void Buffer::setBodyPtr(quint8 *bodyPtr)
{
    Q_ASSERT( (bodyPtr == 0) ||
              ((bodyPtr >= _buffer) && (bodyPtr <= (_buffer+_buffLength))) );
    _bodyPtr = bodyPtr;
}

quint8 *Buffer::bodyPtr()
{
    return _bodyPtr;
}

bool Buffer::isValid() const
{
    //true if _refCount is not pointing to nothing (to 0)
    return (0 != _refCount);
}

bool Buffer::isShared() const
{
    return (1 != (*_refCount));
}

void Buffer::printArray(quint8 *ptr, int size, const char *pretext)
{
    printf("%s 0x", pretext);
    for (int i=0; i<size; i++) {
        printf("%02x ", ptr[i]);
    }
    printf("\n");
    fflush(stdout);
}
