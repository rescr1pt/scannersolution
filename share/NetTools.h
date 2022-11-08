#pragma once

#include "Types.h"

template <typename T>
inline T const& byteToNumeric(const unsigned char* byteBuff, T& dec, bool toBigEndian = true)
{
    dec = 0;
    if (toBigEndian)
    {
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            dec = (dec << 8) | byteBuff[i];
        }
    }
    else
    {
        for (int i = sizeof(T) - 1; i >= 0; --i)
        {
            dec = (dec << 8) | byteBuff[i];
        }
    }

    return dec;
}

template <typename T>
inline T byteToNumeric(const unsigned char* byteBuff, bool toBigEndian = true)
{
    T dec = 0;
    return byteToNumeric<T>(byteBuff, dec, toBigEndian);
}

template <typename T>
inline char* numericToByte(T src, char* dst)
{
    for (size_t i = 0; i < sizeof(T); ++i)
    {
        dst[sizeof(T) - i - 1] = src >> (8 * i) & 0xFF;
    }

    return dst;
}

class DynamicByteWriter
{
public:
    DynamicByteWriter();
    ~DynamicByteWriter();

    void append(const char* data, size_t dataSize);
    template <typename T> void append(T dec);
    template <typename T> DynamicByteWriter& operator << (T data) { append<T>(data); return *this; }

    size_t getWritedSize() const { return writedSize_; }
    char* getWritedBuffer() const { return buffer_; }

    char* takeBuffer(size_t& outSize);

private:
    char* buffer_;
    size_t writedSize_;
    size_t bufferCapacity_;
};

template <typename T>
void DynamicByteWriter::append(T dec)
{
    const ub2 sizeInBytes = sizeof(dec);
    char buff[8];

    numericToByte<T>(dec, buff);
    append(buff, sizeInBytes);
}
