#pragma once

#include <shared_mutex>
#include <chrono>

#include "ThreadPoolQueue.h"
#include "IScanExecutor.h"
#include "ScannerData.h"

class ScannerLocalCtxCreator;
class ScannedHostResultWritter;

class ScannerLocalCtx : public IThreadPoolLocalContext
{
public:
    ScannerLocalCtx(ScannerLocalCtxCreator& creator, size_t threadIndex);

    void init();

public:
    ScannerLocalCtxCreator& creator_;
    size_t threadIndex_ = 0;

    std::unique_ptr<IScanExecutor> scanExecutor_;
    std::unique_ptr<ScannedHostResultWritter> scannedResultWritter_;
};

class ScannerLocalCtxCreator : public IThreadPoolLocalContextCreator
{
public:
    ScannerLocalCtxCreator(ScannerData& scannerData);

    std::unique_ptr<IThreadPoolLocalContext> create(size_t threadIndex) override;
    void periodLastScannedHost(const IpData& ipData);

    ScannerData& scannerData_;
    ScanExecutorOptions executerOptions_;
    std::chrono::system_clock::time_point lastScannedFlushPeriodTimer_;
    std::shared_mutex scannedflushMutex_;
};

class ScannerTPExecutorCommand : public IThreadPoolCommand
{
public:
    ScannerTPExecutorCommand(const TargetData::PortRanges& ports, const IpData& host);

    ~ScannerTPExecutorCommand();

    void onPopQueue(IThreadPoolLocalContext* threadLocalContext) override;
    void exec(IThreadPoolLocalContext* threadLocalContext) override;

private:

    void execPort(ScannerLocalCtx& threadCtx, IScanExecutor& executor, ub2 port);

private:
    const TargetData::PortRanges& ports_;
    IpData host_;
};

class ScannedHostResultWritter
{
public:
    ScannedHostResultWritter(const std::string& folderPath, size_t threadIndex);

    void write(ScanExecutorExecutedStage stage, const IpData& ipData, ub2 port);
    void forceFlush();

private:
    std::ofstream fout_;
    std::string ipDataStr_;
};

class Scanner
{
public:
    Scanner(const std::string& configFile);

    ~Scanner();

    void run();

    ScannerData scannerData_;
    ThreadPoolQueue tp_;
};