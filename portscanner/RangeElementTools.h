#pragma once

#include "FillStringRangersToElementRanges.hpp"
#include "ElementRangeNext.hpp"

struct IpData;

void stringToIpData(IpData& val, const std::string& byStr);

ub4& ipDataToDecimal(IpData& val);

void stringToPort(ub2& val, const std::string& byStr);

ub2& portToDecimal(ub2& val);

void fillStringRangesToIpDataRanges(const ElementRanges<std::string>& stringRanges
    , ElementRanges<IpData>& elRangesSkip
    , ElementRanges<IpData>& elRanges);

void fillStringRangesToPortRanges(const ElementRanges<std::string>& stringRanges
    , ElementRanges<ub2>& elRangesSkip
    , ElementRanges<ub2>& elRanges);

class IpDataRangeNext : public ElementRangeNext<IpData, ub4>
{
public:
    explicit IpDataRangeNext(ElementRanges<IpData>& ipDataRanges);
};

class PortRangeNext : public ElementRangeNext<ub2, ub2>
{
public:
    explicit PortRangeNext(ElementRanges<ub2>& portRanges);
};