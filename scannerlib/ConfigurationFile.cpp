#include "ConfigurationFile.h"
#include "StrTokenizer.h"
#include "StrTools.h"

ConfigurationFile::ConfigurationFile(const std::string& fileName) : CushyConfig(fileName)
{
    addParameter("config_version", parameters_.configVersion_);
    addParameter("trace_level_mask", parameters_.traceLevelMask_);
    addParameter("connecting_timeout", parameters_.connectingTimeout_);
    addParameter("io_timeout", parameters_.ioTimeout_);
    addParameter("thread_opts", parameters_.threadOpts_);
    addParameter("write_result_stage", parameters_.writeResultStage_);
    addParameter("scanned_flush_period", parameters_.scannedFlushPeriod_);
    addParameter("output_folder", parameters_.outputFolder_);
    addParameter("instance_type", parameters_.instanceType_);
    addParameter("host_ranges", parameters_.hostRanges_);
    addParameter("host_ranges_skip", parameters_.hostRangesSkip_);
    addParameter("port_ranges", parameters_.portRanges_);
}

const ConfigurationFile::ConfigParameters& ConfigurationFile::getParameters() const
{
    return parameters_;
}

const std::string ConfigurationFile::StringRangeType::getAsString() const
{
    std::string str;

    for (auto& h : stringRanges_.elements_) {
        if (!str.empty()) {
            str += ",";
        }

        str += h;
    }

    for (auto& h : stringRanges_.ranges_) {
        if (!str.empty()) {
            str += ",";
        }

        str += h.first;
        str += "-";
        str += h.second;
    }

    return std::move(str);
}

void ConfigurationFile::StringRangeType::setAsString(const std::string& value)
{
    StrTokenizer pairsTokenizers;
    pairsTokenizers.set(value.c_str(), value.size(), ",");

    StrTokenizer rangeTokenizer;

    while (pairsTokenizers.next()) {
        rangeTokenizer.set(pairsTokenizers.getToken(), pairsTokenizers.getTokenSize(), "-");
        if (!rangeTokenizer.next()) {
            throw Exception("Invalid type of range-string parameter " + value);
        }

        std::string strStart = rangeTokenizer.getTokenAsString();
        strut::trim(strStart);

        // Second token not found
        if (!rangeTokenizer.next()) {
            stringRanges_.elements_.emplace_back(strStart);
            continue;
        }

        std::string strEnd = rangeTokenizer.getTokenAsString();


        strut::trim(strEnd);

        stringRanges_.ranges_.emplace_back(strStart, strEnd);
    }
}

const std::string ConfigurationFile::DecimalPairType::getAsString() const
{
    std::string str = std::to_string(first_);
    str += ",";
    str += std::to_string(second_);
    return std::move(str);
}

void ConfigurationFile::DecimalPairType::setAsString(const std::string& value)
{
    StrTokenizer tokenizer;
    tokenizer.set(value.c_str(), value.size(), ",");

    if (!tokenizer.next()) {
        throw Exception("Invalid parameter for seconds. Param " + value);
    }

    try
    {
        first_ = std::stoul(tokenizer.getTokenAsString());
    }
    catch (const std::runtime_error&)
    {
        throw Exception("Invalid parameter for seconds. Param " + value);
    }

    // Microseconds not set
    if (!tokenizer.next()) {
        return;
    }

    try
    {
        second_ = std::stoul(tokenizer.getTokenAsString());
    }
    catch (const std::runtime_error&)
    {
        throw Exception("Invalid parameter for microseconds. Param " + value);
    }
}
