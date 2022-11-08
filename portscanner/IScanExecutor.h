#pragma once

#include <string>
#include "Types.h"
#include "Networking.h"
#include "ScanExecutorExecutedStage.h"

struct IpData;

struct ScanExecutorOptions
{
    ScanExecutorOptions() {}
    nw::Options nwOpts_;
};

class IScanExecutor
{
public:
    IScanExecutor(const ScanExecutorOptions& opts) 
        : opts_(opts)
    {}

    virtual ~IScanExecutor() {}

    virtual ScanExecutorExecutedStage exec(const IpData& ipData, ub2 port) = 0;

private:
    const ScanExecutorOptions& opts_;
};