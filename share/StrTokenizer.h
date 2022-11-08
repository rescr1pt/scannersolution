#pragma once

#include "Types.h"
#include <string>

class StrTokenizer
{
public:
    StrTokenizer();
    ~StrTokenizer();

    void set(const char* str, size_t strSize, const char* delimiterList);
    void setDelimiterList(const char* delimiterList);

    const char* getBuffer() const { return buffer_; }
    size_t getBufferSize() const { return bufferSize_; }

    size_t getStartPos() const { return startPos_; }
    sb4 getCurrentPos() const { return currentPos_; }

    const char* getToken() const { return token_; }
    size_t getTokenSize() const { return tokenSize_; }
    std::string getTokenAsString() const;

    char getLastDelimiter() const { return lastDelimiter_; }

    const char* next();
    const char* prev();

    void switchToFront() { currentPos_ = static_cast<sb4>(startPos_); }
    void switchToBack() { currentPos_ = static_cast<sb4>(bufferSize_); }

private:
    bool isDelimiter(char c);

private:
    const char* buffer_ = nullptr;
    size_t bufferSize_ = 0;
    const char* delimiterSet_ = nullptr;
    const char* token_ = nullptr;
    size_t tokenSize_ = 0;
    size_t startPos_ = 0;
    sb4 currentPos_ = 0;
    char lastDelimiter_ = '\0';
};