#include "Scanner.h"
#include "ScanExecutorPostgreSQL.h"
#include "RangeElementTools.h"
#include "ScannerTracer.h"

ScannerLocalCtx::ScannerLocalCtx(ScannerLocalCtxCreator& creator, size_t threadIndex)
    : creator_(creator)
    , threadIndex_(threadIndex)
{
}

void ScannerLocalCtx::init()
{
    const auto& instanceRow = creator_.scannerData_.getTargetData();

    scannedResultWritter_ = std::make_unique<ScannedHostResultWritter>(creator_.scannerData_.getPaths().resultFolder_, threadIndex_);

    if (instanceRow.instanceType_ == InstanceType::PostgreSQL) {
        scanExecutor_ = std::make_unique<ScanExecutorPostgreSQL>(creator_.executerOptions_);
    }
}

void ScannerLocalCtxCreator::periodLastScannedHost(const IpData& ipData)
{
    auto now = std::chrono::system_clock::now();

    auto checkToFlush = [&]() -> bool
    {
        return now - lastScannedFlushPeriodTimer_ > scannerData_.getTargetData().scannedFlushPerioud_;
    };

    {
        std::shared_lock<std::shared_mutex> guard(scannedflushMutex_);

        // Check and return for fast unlock
        if (!checkToFlush()) {
            return;
        }
    }

    std::unique_lock<std::shared_mutex> guard(scannedflushMutex_);

    if (!checkToFlush()) {
        return;
    }
    
    lastScannedFlushPeriodTimer_ = now;
    scannerData_.flushLastScannedHost(ipData);
}

ScannerLocalCtxCreator::ScannerLocalCtxCreator(ScannerData& scannerData) : scannerData_(scannerData)
{
    lastScannedFlushPeriodTimer_ = std::chrono::system_clock::now();

    executerOptions_.nwOpts_.connectingTimeout_ = scannerData_.getTargetData().connectingTimeout_;
    executerOptions_.nwOpts_.ioTimeout_ = scannerData_.getTargetData().ioOperationTimeout_;
}

std::unique_ptr<IThreadPoolLocalContext> ScannerLocalCtxCreator::create(size_t threadIndex)
{
    std::unique_ptr<ScannerLocalCtx> ctx = std::make_unique<ScannerLocalCtx>(*this, threadIndex);
    ctx->init();
    return ctx;
}

ScannerTPExecutorCommand::ScannerTPExecutorCommand(const TargetData::PortRanges& ports, const IpData& host) 
    : ports_(ports)
    , host_(host)
{

}

ScannerTPExecutorCommand::~ScannerTPExecutorCommand()
{

}

void ScannerTPExecutorCommand::onPopQueue(IThreadPoolLocalContext* threadLocalContext)
{
    ScannerLocalCtx& threadCtx = static_cast<ScannerLocalCtx&>(*threadLocalContext);
    threadCtx.creator_.periodLastScannedHost(host_);
}

void ScannerTPExecutorCommand::exec(IThreadPoolLocalContext* threadLocalContext)
{
    ScannerLocalCtx& threadCtx = static_cast<ScannerLocalCtx&>(*threadLocalContext);

    auto portCopyies = ports_;

    PortRangeNext portRangeNext(portCopyies);

    while (!portRangeNext.isEof()) {
        execPort(threadCtx, *threadCtx.scanExecutor_, portRangeNext.next());
    }
}

void ScannerTPExecutorCommand::execPort(ScannerLocalCtx& threadCtx, IScanExecutor& executor, ub2 port)
{
    ScanExecutorExecutedStage stage = executor.exec(host_, port);
    const auto& instRow = threadCtx.creator_.scannerData_.getTargetData();
    
    ub2 configStage = static_cast<ub2>(instRow.writeResultStage_);
    ub2 curStage = static_cast<ub2>(stage);

    if (curStage >= configStage) {
        SCANNER_TRACE_INFO("%s state=%u, host = %s, port = %u \n", __FUNCTION__, stage, host_.toStr().c_str(), port);

        threadCtx.scannedResultWritter_->write(stage, host_, port);

        if (stage == instRow.writeResultStage_) {
            threadCtx.scannedResultWritter_->forceFlush();
        }
    }
}

Scanner::Scanner(const std::string& configFile) : scannerData_(configFile)
{

}

Scanner::~Scanner()
{
    tp_.stop(true);
}

void Scanner::run()
{
    try
    {
        scannerData_.load();
    }
    catch (const std::exception& e)
    {
        printf("Can not load config file. %s\n", e.what());
        return;
    }

    const auto& instRow = scannerData_.getTargetData();

    std::string traceFileFullPath = scannerData_.getPaths().logsFolder_ + "/" + "log.txt";
    const bool tracerIsOpened = ScannerTracer::self().open(traceFileFullPath, instRow.traceLevelMask_, 4096, std::make_pair(true, true));

    if (!tracerIsOpened) {
        printf("Can not open trace file %s\n", traceFileFullPath.c_str());
        return;
    }

    SCANNER_TRACE_INFO("Tracer file opened\n");

    ScannerLocalCtxCreator creator(scannerData_);

    try
    {
        tp_.start(instRow.threadOpts_.first, instRow.threadOpts_.second, &creator);
    }
    catch (const std::exception& e)
    {
        SCANNER_TRACE_ERROR("%s\n", e.what());
        return;
    }

    IpDataRangeNext ipRangeNext(scannerData_.getIpDataRanges());
    
    while (!ipRangeNext.isEof()) {
        IpData targetIpData = ipRangeNext.next();
        std::unique_ptr<IThreadPoolCommand> cmd = std::make_unique<ScannerTPExecutorCommand>(scannerData_.getTargetData().portRanges_, targetIpData);
        tp_.putCommand(std::move(cmd), false);
    }

    tp_.stop(false);
}

ScannedHostResultWritter::ScannedHostResultWritter(const std::string& folderPath, size_t threadIndex)
{
    std::string filePath = folderPath + "/" + std::to_string(threadIndex) + ".txt";
    fout_.open(filePath, std::ios_base::app);

    if (!fout_.is_open()) {
        throw std::runtime_error("Can not open result " + filePath);
    }
}

void ScannedHostResultWritter::write(ScanExecutorExecutedStage stage, const IpData& ipData, ub2 port)
{
    ipDataStr_.clear();
    ipData.toStr(ipDataStr_);

    fout_ << static_cast<ub2>(stage) << ',' << ipDataStr_ << ',' << port << std::endl;
}

void ScannedHostResultWritter::forceFlush()
{
    fout_.flush();
}
