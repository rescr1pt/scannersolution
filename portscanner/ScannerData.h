#pragma once

#include <string>
#include <chrono>

#include "InstanceType.h"
#include "TargetData.h"
#include "IpData.h"
#include "ConfigurationFile.h"
#include "IScanExecutor.h"


class LastScannedHostRW
{
public:
    // RAII
    explicit LastScannedHostRW(const std::string& fileName);
    ~LastScannedHostRW();

    bool write(const IpData& ipData);
    const IpData& getReadedLastScannedHost() const { return readedLastScannedHost_; }

private:
    std::string fileName_;
    std::ofstream file_;
    IpData readedLastScannedHost_;
};

class ScannerData
{
public:
    struct Paths
    {
        std::string rootFolder_;
        std::string logsFolder_;
        std::string resultFolder_;
        std::string lastScannedHost_;
    };

public:
    ScannerData(const std::string& file);
    ~ScannerData();

    void load();

    const TargetData& getTargetData() const;
    const ConfigurationFile& getConfigurationFile() const;
    const Paths getPaths() const;
    ElementRanges<IpData>& getIpDataRanges() { return instanceRow_.hostRanges_; }

    void flushLastScannedHost(const IpData& ipData);

private:

    void loadInstance();

    void loadInstanceHosts();

    void loadInstancePorts();

private:
    ConfigurationFile configFile_;
    TargetData instanceRow_;
    Paths paths_;

    std::unique_ptr<LastScannedHostRW> lastScannedHostRW_;
};
