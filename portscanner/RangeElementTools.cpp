#include "RangeElementTools.h"
#include "IpData.h"

void stringToIpData(IpData& val, const std::string& byStr)
{
    val.fromStr(byStr);
}

ub4& ipDataToDecimal(IpData& val)
{
    // self type is nativeType
    return val.data_.dec_;
}

void stringToPort(ub2& val, const std::string& byStr)
{
    val = static_cast<ub2>(std::stoul(byStr));
}

ub2& portToDecimal(ub2& val)
{
    // selftype is nativeType, use const cast
    return const_cast<ub2&>(val);
}

void fillStringRangesToIpDataRanges(const ElementRanges<std::string>& stringRanges, ElementRanges<IpData>& elRangesSkip, ElementRanges<IpData>& elRanges)
{
    fillStringRangesToElementRanges<IpData, ub4>(stringRanges, elRangesSkip, elRanges, stringToIpData, ipDataToDecimal);

    for (auto& el : elRanges.ranges_) {
        el.first.updateDebugView();
        el.second.updateDebugView();
    }

    for (auto& el : elRanges.elements_) {
        el.updateDebugView();
    }
}

void fillStringRangesToPortRanges(const ElementRanges<std::string>& stringRanges, ElementRanges<ub2>& elRangesSkip, ElementRanges<ub2>& elRanges)
{
    fillStringRangesToElementRanges<ub2, ub2>(stringRanges, elRangesSkip, elRanges, stringToPort, portToDecimal);
}

PortRangeNext::PortRangeNext(ElementRanges<ub2>& portRanges) 
    : ElementRangeNext<ub2, ub2>(portRanges, portToDecimal)
{

}

IpDataRangeNext::IpDataRangeNext(ElementRanges<IpData>& ipDataRanges) 
    : ElementRangeNext<IpData, ub4>(ipDataRanges, ipDataToDecimal)
{

}
