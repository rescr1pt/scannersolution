#include "StrTokenizer.h"

StrTokenizer::StrTokenizer()
{
}

StrTokenizer::~StrTokenizer()
{
}

void StrTokenizer::set(const char* str, size_t strSize, const char* delimiterList)
{
    startPos_ = tokenSize_ = 0;
    currentPos_ = 0;
    buffer_ = str;
    bufferSize_ = strSize;
    setDelimiterList(delimiterList);
    token_ = nullptr;
    lastDelimiter_ = '\0';
}

void StrTokenizer::setDelimiterList(const char* delimiters)
{
    delimiterSet_ = delimiters;
}

std::string StrTokenizer::getTokenAsString() const
{
    return token_ 
        ? std::string(getToken(), getTokenSize()) 
        : "";
}

const char* StrTokenizer::next()
{
    if (buffer_ == nullptr || currentPos_ >= bufferSize_)
    {
        token_ = nullptr;
        return token_;
    }


    while (bufferSize_ > currentPos_ && isDelimiter(buffer_[currentPos_]))
    {
        ++currentPos_;
    }

    lastDelimiter_ = buffer_[currentPos_ - 1];

    if (currentPos_ >= bufferSize_)
    {
        token_ = nullptr;
        return token_;
    }

    size_t endPos = currentPos_;

    while (bufferSize_ > endPos && !isDelimiter(buffer_[endPos]))
    {
        ++endPos;
    }

    tokenSize_ = endPos - currentPos_;

    currentPos_ = (sb4)endPos;
    token_ = buffer_ + currentPos_ - tokenSize_;

    return token_;
}

const char* StrTokenizer::prev()
{
    if (!buffer_ || currentPos_ <= startPos_)
    {
        token_ = nullptr;
        return token_;
    }

    if (currentPos_ == bufferSize_)
    {
        --currentPos_;
    }

    while (currentPos_ > -1 && isDelimiter(buffer_[currentPos_]))
    {
        --currentPos_;
    }

    lastDelimiter_ = buffer_[currentPos_ - 1];

    if (currentPos_ <= -1)
    {
        token_ = nullptr;
        return token_;
    }

    sb4 beginPos = currentPos_;

    while (beginPos > -1 && !isDelimiter(buffer_[beginPos]))
    {
        --beginPos;
    }

    tokenSize_ = currentPos_ - beginPos;
    token_ = buffer_ + beginPos + 1;
    currentPos_ = beginPos;

    return token_;
}

bool StrTokenizer::isDelimiter(char c)
{
    if (c == '\0')
    {
        return true;
    }

    for (int i = 0; delimiterSet_[i] != '\0'; ++i)
    {
        if (delimiterSet_[i] == c)
        {
            return true;
        }
    }

    return false;
}

