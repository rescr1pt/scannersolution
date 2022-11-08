#pragma once

#include <list>
#include <chrono>

#include "Types.h"
#include "IpData.h"
#include "InstanceType.h"
#include "ScanExecutorExecutedStage.h"
#include "ElementRanges.h"

struct TargetData
{
    typedef std::pair<ub4, ub4> TimeVal; // seconds, microseconds
    typedef std::pair<IpData, IpData> IpDataRange;
    typedef std::pair<ub2, ub2> PortRange;
    typedef std::pair<ub2, ub4> ThreadOpts;

    typedef ElementRanges<IpData> HostRanges;
    typedef ElementRanges<ub2> PortRanges;

    ub4 configVersion_ = 1;
    ub2 traceLevelMask_ = 0;
    TimeVal connectingTimeout_ = { 1, 0 };
    ub4 ioOperationTimeout_ = 1000; // milliseconds
    ThreadOpts threadOpts_ = { 40, 10000 };
    ScanExecutorExecutedStage writeResultStage_ = ScanExecutorExecutedStage::Connected;
    std::chrono::seconds scannedFlushPerioud_ = std::chrono::seconds(8);
    std::string outputFolder_;
    InstanceType instanceType_;
    HostRanges hostRanges_;
    PortRanges portRanges_;
    HostRanges hostRangesSkip_;
};
