#include "IpData.h"



IpData::IpData(const IpData& ipData)
{
    fromIpData(ipData);
}

IpData::IpData()
{

}

IpData::IpData(const char* str, size_t strSize)
{
    fromStr(str, strSize);
}

IpData::IpData(const std::string& str)
{
    fromStr(str);
}

IpData::IpData(const char* str)
{
    fromStr(str);
}

IpData::IpData(const ub4 decimal)
{
    fromDecimal(decimal);
}

void IpData::fromIpData(const IpData& ipData)
{
    fromDecimal(ipData.data_.dec_);
}

void IpData::fromDecimal(const ub4 dec)
{
    this->data_.dec_ = dec;
    updateDebugView();
}

bool IpData::fromStr(const std::string& str)
{
    return fromStr(str.c_str(), str.size());
}

bool IpData::fromStr(const char* str, size_t strSize)
{
    size_t index = 0;
    size_t startPos = 0;
    char buffer[4];
    int dec = 0;
    for (size_t i = 0; i < strSize; ++i) {
        if (str[i] == '.' || i + 1 == strSize) {
            if (i + 1 == strSize) {
                if (index != 3) {
                    return false;
                }

                memcpy(buffer, str + startPos, (i + 1) - startPos);
                buffer[(i + 1) - startPos] = 0;
            }
            else {
                memcpy(buffer, str + startPos, i - startPos);
                buffer[i - startPos] = 0;
                startPos = i + 1;
            }

            dec = atoi(buffer);
            if (dec > 255) {
                return false;
            }

            // write to union
            data_.bt_[3 - index] = dec;
            ++index;

            if (index == 4) {
                break;
            }
            else {
                continue;
            }
        }

        if (i - startPos > 3) {
            return false;
        }
    }

    updateDebugView();

    return true;
}

void IpData::toStr(std::string& str) const
{
    char buffer[4];

    for (size_t i = 0; i < 4; ++i) {
        _itoa_s(data_.bt_[3 - i], buffer, sizeof(buffer), 10);

        if (!str.empty()) {
            str += '.';
        }

        str += buffer;
    }
}

std::string IpData::toStr() const
{
    std::string str;
    toStr(str);
    return str;
}

ub4 IpData::toInetAddr() const
{
    return (data_.bt_[3] << 0) | (data_.bt_[2] << 8) | (data_.bt_[1] << 16) | ((unsigned)data_.bt_[0] << 24);
}

void IpData::updateDebugView()
{
#ifdef _DEBUG
    std::string str;
    toStr(str);
    std::memcpy(debugView_, str.c_str(), str.size());
    debugView_[str.size()] = 0;
#endif
}
