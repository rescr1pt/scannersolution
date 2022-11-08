#include "ScanExecutorPostgreSQL.h"
#include "Networking.h"
#include "NetTools.h"


ScanExecutorPostgreSQL::ScanExecutorPostgreSQL(const ScanExecutorOptions& opts)
    : IScanExecutor(opts)
{
    client_ = std::make_unique<nw::Client>(opts.nwOpts_);
}

ScanExecutorExecutedStage ScanExecutorPostgreSQL::exec(const IpData& ipData, ub2 port)
{
    static const unsigned char SSLStartup[8] =
    {
        0x00, 0x00, 0x00, 0x08, 0x04, 0xd2, 0x16, 0x2f
    };

    static const char* pSSLStartup = (const char*)SSLStartup;
    static const int SSLStartupSize = sizeof(SSLStartup);

    ScanExecutorExecutedStage stage = ScanExecutorExecutedStage::None;

    if (!client_->connect(ipData, port)) {
        return stage;
    }

    stage = ScanExecutorExecutedStage::Connected;

    if (!client_->sendPacket(pSSLStartup, SSLStartupSize)) {
        return stage;
    }

    if (!client_->recvPacket(outBuffer_, 1)) {
        return stage;
    }

    client_->disconnect();

    if (*outBuffer_ == 'S'
        || *outBuffer_ == 'N'
        || *outBuffer_ == 'G') {
        stage = ScanExecutorExecutedStage::ProtocolChecked;
        return stage;
    }
    else {
        return stage;
    }
}
