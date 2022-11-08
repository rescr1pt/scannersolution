#include "NetTools.h"
#include <cstring>


DynamicByteWriter::DynamicByteWriter()
    : buffer_(nullptr)
    , writedSize_(0)
    , bufferCapacity_(16)
{
    buffer_ = new char[bufferCapacity_];
}

DynamicByteWriter::~DynamicByteWriter()
{
    delete[]buffer_;
}

void DynamicByteWriter::append(const char* data, size_t dataSize)
{
    if (dataSize == 0)
    {
        return;
    }

    const size_t totalWrited = writedSize_ + dataSize;

    // Realloc
    if (totalWrited + 1 > bufferCapacity_)
    {
        bufferCapacity_ = (totalWrited + (bufferCapacity_ / 2)) + 1;

        char* allocatedBuffer = new char[bufferCapacity_];
        memcpy(allocatedBuffer, buffer_, writedSize_);
        delete[]buffer_;
        buffer_ = allocatedBuffer;
    }

    // Overlap is possible because the copy is linear.
    // http://stackoverflow.com/questions/28623895/why-is-memmove-faster-than-memcpy
    // http://vgable.com/blog/2008/05/24/memcopy-memmove-and-speed-over-safety/
    memmove(buffer_ + writedSize_, data, dataSize);
    writedSize_ = totalWrited;
    buffer_[writedSize_] = '\0';
}

char* DynamicByteWriter::takeBuffer(size_t& outSize)
{
    char* tmp = buffer_;
    outSize = writedSize_;

    bufferCapacity_ = 16;
    buffer_ = new char[bufferCapacity_];
    writedSize_ = 0;

    return tmp;
}


