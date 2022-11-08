#include "ScannerData.h"
#include "RangeElementTools.h"
#include "FileTools.h"
#include "ScannerTracer.h"

ScannerData::ScannerData(const std::string& file) 
    : configFile_(file)
{

}

ScannerData::~ScannerData()
{
}

void ScannerData::load()
{
    configFile_.load();

    loadInstance();
}

const TargetData& ScannerData::getTargetData() const
{
    return instanceRow_;
}

const ConfigurationFile& ScannerData::getConfigurationFile() const
{
    return configFile_;
}

const ScannerData::Paths ScannerData::getPaths() const
{
    return paths_;
}

void ScannerData::flushLastScannedHost(const IpData& ipData)
{
    bool isWritten = lastScannedHostRW_->write(ipData);

    if (isWritten) {
        SCANNER_TRACE_INFO("%s: Flushed the last scanned host %s\n", __FUNCTION__, ipData.toStr().c_str());
    }
    else {
        SCANNER_TRACE_ERROR("%s: Can not flush the last scanned host %s\n", __FUNCTION__, ipData.toStr().c_str());
    }
}

void ScannerData::loadInstance()
{
    auto& params = configFile_.getParameters();

    instanceRow_.configVersion_ = params.configVersion_;
    instanceRow_.traceLevelMask_ = params.traceLevelMask_;
    instanceRow_.threadOpts_ = std::make_pair(params.threadOpts_.first_, params.threadOpts_.second_);
    instanceRow_.writeResultStage_ = static_cast<ScanExecutorExecutedStage>(params.writeResultStage_);
    instanceRow_.scannedFlushPerioud_ = std::chrono::seconds(params.scannedFlushPeriod_);
    instanceRow_.outputFolder_ = params.outputFolder_;
    instanceRow_.instanceType_ = static_cast<InstanceType>(params.instanceType_);
    instanceRow_.connectingTimeout_ = std::make_pair(params.connectingTimeout_.first_, params.connectingTimeout_.second_);
    instanceRow_.ioOperationTimeout_ = params.ioTimeout_;

    std::string outputFolder = getTargetData().outputFolder_;
    auto outfolderIsInvalidIt = std::find_if(outputFolder.begin(), outputFolder.end(), [](char c) { return !std::isalnum(c) && c == '_'; });
    if (outfolderIsInvalidIt != outputFolder.end()) {
        throw std::runtime_error("Invalid format for output_folder " + outputFolder);
    }

    static const std::string ROOT_PREFIX_STR = "portscanner_";
    static const std::string LOGS_FOLDER_STR = "logs";
    static const std::string RESULT_FOLDER_STR = "result";
    static const std::string LAST_SCANNED_FILE_STR = "last_scanned.txt";

    paths_.rootFolder_ = "portscanner_" + outputFolder;
    paths_.logsFolder_ = paths_.rootFolder_ + "/" + LOGS_FOLDER_STR;
    paths_.resultFolder_ = paths_.rootFolder_ + "/" + RESULT_FOLDER_STR;
    paths_.lastScannedHost_ = paths_.rootFolder_ + "/" + LAST_SCANNED_FILE_STR;

    auto createDir = [](const std::string& folder)
    {
        if (!fileut::createFolderRecursive(folder, true)) {
            throw std::runtime_error("Cannot create folder " + folder);
        }
    };

    createDir(paths_.rootFolder_);
    createDir(paths_.logsFolder_);
    createDir(paths_.resultFolder_);

    lastScannedHostRW_ = std::make_unique<LastScannedHostRW>(paths_.lastScannedHost_);

    loadInstanceHosts();
    loadInstancePorts();
}

void ScannerData::loadInstanceHosts()
{
    auto& params = configFile_.getParameters();
    auto& hostRanges = instanceRow_.hostRanges_;
    auto& hostRangesSkip = instanceRow_.hostRangesSkip_;


    // fill skip ranges
    for (auto& sRange : params.hostRangesSkip_.stringRanges_.ranges_) {
        hostRangesSkip.ranges_.push_back(ElementRanges<IpData>::Range(sRange.first, sRange.second));
    }
    for (auto& sElement : params.hostRangesSkip_.stringRanges_.elements_) {
        hostRangesSkip.elements_.push_back(sElement);
    }

    const auto& lastScannedHostDec = lastScannedHostRW_->getReadedLastScannedHost();
    if (lastScannedHostDec.data_.dec_ != 0) {
        // skip before
        hostRangesSkip.ranges_.emplace_back("0.0.0.1", lastScannedHostDec);
    }

    fillStringRangesToIpDataRanges(params.hostRanges_.stringRanges_, hostRangesSkip, hostRanges);
}

void ScannerData::loadInstancePorts()
{
    auto& params = configFile_.getParameters();
    auto& portRanges = instanceRow_.portRanges_;
    TargetData::PortRanges portRangesSkip; // empty

    fillStringRangesToPortRanges(params.portRanges_.stringRanges_, portRangesSkip, portRanges);
}

LastScannedHostRW::LastScannedHostRW(const std::string& fileName)
    : fileName_(fileName)
{
    // read filename
    std::ifstream fin(fileName);

    if (fin.is_open()) {
        std::string line;
        if (std::getline(fin, line)) {
            if (!readedLastScannedHost_.fromStr(line)) {
                throw std::runtime_error("Can not transform the 'lastScannedHostName' from " + line + " to ip string");
            }
        }

        fin.close();
    }
}

LastScannedHostRW::~LastScannedHostRW()
{
    if (file_.is_open()) {
        file_.close();
    }
}

bool LastScannedHostRW::write(const IpData& lastScannedHost)
{
    file_.open(fileName_, std::ofstream::out | std::ofstream::trunc);
    if (!file_.is_open()) {
        throw std::runtime_error("Can not open file " + fileName_);
    }

    std::string strData;
    lastScannedHost.toStr(strData);

    file_ << strData;
    file_.close();

    return true;
}
