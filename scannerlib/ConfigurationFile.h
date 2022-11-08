#pragma once

#include "CushyConfig.h"
#include "Types.h"
#include "ElementRanges.h"

class ConfigurationFile : public CushyConfig
{
    struct DecimalPairType : ICustomType
    {
        virtual const std::string getAsString() const override;

        void setAsString(const std::string& value) override;

        ub4 first_ = 0;
        ub4 second_ = 0;
    };


    struct StringRangeType : ICustomType
    {
        typedef std::pair<std::string, std::string> StringRange;

        virtual const std::string getAsString() const override;

        void setAsString(const std::string& value) override;

        ElementRanges<std::string> stringRanges_;
    };

    struct ConfigParameters
    {
        ub4 configVersion_;
        ub2 traceLevelMask_;
        DecimalPairType connectingTimeout_;
        ub4 ioTimeout_;
        DecimalPairType threadOpts_;
        ub2 writeResultStage_;
        ub4 scannedFlushPeriod_;
        std::string outputFolder_;
        ub2 instanceType_;
        StringRangeType hostRanges_;
        StringRangeType hostRangesSkip_;
        StringRangeType portRanges_;
    };

public:

    ConfigurationFile(const std::string& fileName);

    const ConfigParameters& getParameters() const;

private:
    ConfigParameters parameters_;
};