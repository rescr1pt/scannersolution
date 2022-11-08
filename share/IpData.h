#pragma once

#include "Types.h"
#include <string>

struct IpData
{
    IpData();
    IpData(const IpData& ipData);
    IpData(const ub4 decimal);
    IpData(const char* str);
    IpData(const char* str, size_t strSize);
    IpData(const std::string& str);

    bool operator==(const IpData& an) const { return data_.dec_ == an.data_.dec_; }

    void fromIpData(const IpData& ipData);
    void fromDecimal(const ub4 dec);
    bool fromStr(const char* str, size_t strSize);
    bool fromStr(const std::string& str);

    void toStr(std::string& str) const;
    std::string toStr() const;
    ub4 toInetAddr() const;

    // for watching debugView_
    void updateDebugView();

    // BigEndian data
    union Data
    {
        ub1 bt_[4];
        ub4 dec_;
    };
    Data data_ = { 0 };

private:
#if _DEBUG // prettify only for debug
    char debugView_[16];
#endif
};

// will expand for IpV6
#ifdef _DEBUG
static_assert(sizeof(IpData) <= (4 + 16), "Check sizeof"); // debugview
#else
static_assert(sizeof(IpData) <= 4, "Check sizeof");
#endif