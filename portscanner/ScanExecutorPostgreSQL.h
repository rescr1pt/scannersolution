#pragma once

#include "IScanExecutor.h"
#include "memory"

namespace nw
{
    class Client;
}

class ScanExecutorPostgreSQL : public IScanExecutor
{
public:
    ScanExecutorPostgreSQL(const ScanExecutorOptions& opts);

    ScanExecutorExecutedStage exec(const IpData& ipData, ub2 port) override;

private:
    std::unique_ptr<nw::Client> client_;
    char outBuffer_[1] = { 0 };
};