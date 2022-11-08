#include <string>

#include "TargetData.h"
#include "NativeTestEngine.h"
#include "ElementRangeNext.hpp"
#include "RangeElementTools.h"

class IFillRangesTest : protected INativeTest
{
protected:
    template <typename ELEMENT_T>
    void compareOriginalToResults(const ElementRanges<std::string>& original
        , const ElementRanges<ELEMENT_T>& results
        , std::function<std::string(const ELEMENT_T&)> rangeElementToString)
    {
        assertTrue(original.ranges_.size() == results.ranges_.size(), "Original size != Results size");
        assertTrue(original.elements_.size() == results.elements_.size(), "Original size != Results size");

        // check ranges
        {
            auto oIt = original.ranges_.begin();
            auto rIt = results.ranges_.begin();

            for (; oIt != original.ranges_.end(); ++oIt, ++rIt) {
                auto& oFirst = oIt->first;
                auto& oSecond = oIt->second;
                std::string rFirst = rangeElementToString(rIt->first);
                std::string rSecond = rangeElementToString(rIt->second);
                
                assertTrue(oFirst == rFirst, "");
                assertTrue(oSecond == rSecond, "");
            }
        }

        // check elements
        {
            auto oIt = original.elements_.begin();
            auto rIt = results.elements_.begin();

            for (; oIt != original.elements_.end(); ++oIt, ++rIt) {
                auto& oFirst = *oIt;
                std::string rFirst = rangeElementToString(*rIt);

                assertTrue(oFirst == rFirst, "");
            }
        }
    }

    void compareOriginalToResults(const ElementRanges<std::string>& original, const ElementRanges<IpData>& results)
    {
        return compareOriginalToResults<IpData>(original, results, hostToString);
    }

    void compareOriginalToResults(const ElementRanges<std::string>& original, const ElementRanges<ub2>& results)
    {
        return compareOriginalToResults<ub2>(original, results, portToString);
    }

    static std::string hostToString(const IpData& ipData)
    {
        std::string ipDataStr;
        ipData.toStr(ipDataStr);
        return ipDataStr;
    }

    static std::string portToString(const ub2& val)
    {
        return std::to_string(val);
    }

protected:
    ElementRanges<std::string> stringRanges;
};

class FillRangesTest1 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("192.168.1.50", "192.168.1.60");

        stringRanges.ranges_.emplace_back("192.168.1.10", "192.168.1.20");
        stringRanges.ranges_.emplace_back("192.168.1.15", "192.168.1.20"); // skip

        stringRanges.ranges_.emplace_back("192.168.1.30", "192.168.1.40");
        stringRanges.ranges_.emplace_back("192.168.1.39", "192.168.1.40"); // skip



        stringRanges.elements_.emplace_back("192.168.1.50"); // skip
        stringRanges.elements_.emplace_back("192.168.1.60"); // skip
        stringRanges.elements_.emplace_back("192.168.1.90");
        stringRanges.elements_.emplace_back("192.168.1.8");

        ElementRanges<IpData> ipRanges;
        ElementRanges<IpData> skipRanges;

        fillStringRangesToIpDataRanges(stringRanges,  skipRanges, ipRanges);

        ElementRanges<std::string> originalRanges;

        originalRanges.ranges_ =
        {
            { "192.168.1.10", "192.168.1.20" },
            { "192.168.1.30", "192.168.1.40" },
            { "192.168.1.50", "192.168.1.60" }
        };

        originalRanges.elements_ =
        {
            { "192.168.1.8"},
            { "192.168.1.90"},
        };


        compareOriginalToResults(originalRanges, ipRanges);
    }
};

class FillRangesTest2 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("192.168.1.10", "192.168.1.20");
        stringRanges.ranges_.emplace_back("192.168.1.21", "192.168.1.25");
        stringRanges.ranges_.emplace_back("192.168.1.26", "192.168.1.30");
        stringRanges.ranges_.emplace_back("192.168.1.32", "192.168.1.40");
        stringRanges.ranges_.emplace_back("192.168.1.38", "192.168.1.45");
        stringRanges.ranges_.emplace_back("192.168.1.1", "192.168.1.4");
        stringRanges.ranges_.emplace_back("192.168.1.6", "192.168.1.8");

        stringRanges.elements_.emplace_back("192.168.1.5"); 
        stringRanges.elements_.emplace_back("192.168.1.9"); 
        stringRanges.elements_.emplace_back("192.168.1.33"); // skip
        stringRanges.elements_.emplace_back("192.168.1.27"); //skip

        ElementRanges<IpData> ipRanges;
        ElementRanges<IpData> skipRanges;

        fillStringRangesToIpDataRanges(stringRanges,  skipRanges, ipRanges);

        ElementRanges<std::string> originalRanges;

        originalRanges.ranges_ =
        {
            { "192.168.1.1", "192.168.1.4" },
            { "192.168.1.6", "192.168.1.8" },
            { "192.168.1.10", "192.168.1.30" },
            { "192.168.1.32", "192.168.1.45" },
        };

        originalRanges.elements_ =
        {
            { "192.168.1.5"},
            { "192.168.1.9"},
        };


        compareOriginalToResults(originalRanges, ipRanges);

        // part 2

        stringRanges.ranges_.emplace_back("192.168.1.1", "192.168.1.46");
        for (size_t i = 1; i < 47; ++i) {
            stringRanges.elements_.push_back("192.168.1." + std::to_string(i));
        }

        ipRanges.elements_.clear();
        ipRanges.ranges_.clear();

        fillStringRangesToIpDataRanges(stringRanges,  skipRanges, ipRanges);

        originalRanges.ranges_.clear();
        originalRanges.elements_.clear();

        originalRanges.ranges_ =
        {
            { "192.168.1.1", "192.168.1.46" },
        };

        compareOriginalToResults(originalRanges, ipRanges);
    }
};

class FillRangesTest3 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("1005", "1007"); // 6
        stringRanges.ranges_.emplace_back("1000", "1002"); // 4
        stringRanges.ranges_.emplace_back("1003", "1004"); // 5
        stringRanges.ranges_.emplace_back("1011", "64000"); // 8
        stringRanges.ranges_.emplace_back("996", "999"); // 3
        stringRanges.ranges_.emplace_back("992", "995"); // 2
        stringRanges.ranges_.emplace_back("1006", "1009"); //7
        stringRanges.ranges_.emplace_back("990", "991"); // 1

        // skip all
        for (size_t i = 995; i < 1005; ++i) {
            stringRanges.elements_.push_back(std::to_string(i));
        }

        ElementRanges<ub2> portRanges, skipRanges;

        fillStringRangesToPortRanges(stringRanges, skipRanges,  portRanges);

        ElementRanges<std::string> originalRanges;

        originalRanges.ranges_ =
        {
            { "990", "1009" },
            { "1011", "64000" }
        };

        compareOriginalToResults(originalRanges, portRanges);

        // part 2

        for (size_t i = 986; i < 990; ++i) {
            stringRanges.elements_.push_back(std::to_string(i));
        }

        originalRanges.elements_ =
        {
            "986",
            "987",
            "988",
            "989"
        };

        portRanges.ranges_.clear();
        portRanges.elements_.clear();

        fillStringRangesToPortRanges(stringRanges, skipRanges,  portRanges);

        compareOriginalToResults(originalRanges, portRanges);
    }
};

class FillRangesTest4 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("20", "40");

        // 10-19: 20-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(10, 19);

            originalRanges.ranges_ =
            {
                { "20", "40" },
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 20-40: erased
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(20, 40);

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
        }

        // 19-41: erased
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(19, 41);

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
        }

        // 10-25: 26-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(10, 25);
            originalRanges.ranges_ =
            {
                { "26", "40" },
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 41-55: 20-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(41, 55);
            originalRanges.ranges_ =
            {
                { "20", "40" }
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 25-41: 20-24
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(25, 41);
            originalRanges.ranges_ =
            {
                { "20", "24" }
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 25-40: 20-24
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(25, 40);
            originalRanges.ranges_ =
            {
                { "20", "24" }
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 25-30: 20-24, 31-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(25, 30);
            originalRanges.ranges_ =
            {
                { "20", "24" },
                { "31", "40" },
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 25-39: 20-24, 40-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(25, 39);
            originalRanges.ranges_ =
            {
                { "20", "24" },
                { "40", "40" },
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 21-39: 20-20, 40-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.ranges_.emplace_back(21, 39);
            originalRanges.ranges_ =
            {
                { "20", "20" },
                { "40", "40" },
            };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }
    }
};

class FillRangesTest5 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("20", "40");
        stringRanges.elements_.emplace_back("50");
        stringRanges.elements_.emplace_back("51");

        // 19: 20-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.elements_.emplace_back(19);

            originalRanges.ranges_ = 
            {
                { "20", "40" },
            };

            originalRanges.elements_ = { "50", "51" };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 41: 20-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.elements_.emplace_back(41);

            originalRanges.ranges_ =
            {
                { "20", "40" },
            };

            originalRanges.elements_ = { "50", "51" };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 20: 21-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.elements_.emplace_back(20);

            originalRanges.ranges_ =
            {
                { "21", "40" },
            };

            originalRanges.elements_ = { "50", "51" };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 21: 20-20, 22-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.elements_.emplace_back(21);

            originalRanges.ranges_ =
            {
                { "20", "20" },
                { "22", "40" },
            };

            originalRanges.elements_ = { "50", "51" };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 40: 20-39
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.elements_.emplace_back(40);

            originalRanges.ranges_ =
            {
                { "20", "39" },
            };

            originalRanges.elements_ = { "50", "51" };

            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }

        // 50: 20-40
        {
            ElementRanges<ub2> portRanges;
            ElementRanges<ub2> skipPorts;
            ElementRanges<std::string> originalRanges;

            skipPorts.elements_.emplace_back(50);
            skipPorts.elements_.emplace_back(51);

            originalRanges.ranges_ =
            {
                { "20", "40" },
            };


            fillStringRangesToPortRanges(stringRanges, skipPorts, portRanges);
            compareOriginalToResults(originalRanges, portRanges);
        }
    }
};

class FillRangesTest6 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("50.10.20.30", "60.30.15.25");
        stringRanges.ranges_.emplace_back("192.140.20.40", "200.100.10.10");
        stringRanges.ranges_.emplace_back("220.140.20.40", "240.10.20.40");

        stringRanges.elements_.emplace_back("190.10.23.44"); // ok
        stringRanges.elements_.emplace_back("192.140.20.39"); // ok
        stringRanges.elements_.emplace_back("200.100.10.11"); // ok
        stringRanges.elements_.emplace_back("50.10.20.30"); // ignore
        stringRanges.elements_.emplace_back("60.30.15.25"); // ignore
        stringRanges.elements_.emplace_back("240.10.20.40"); // ignore

        ElementRanges<IpData> ranges;
        ElementRanges<IpData> skipIp;
        ElementRanges<std::string> originalRanges;

        skipIp.ranges_.emplace_back("50.10.20.28", "50.10.20.29"); // ignore
        skipIp.ranges_.emplace_back("60.30.15.26", "60.30.15.26"); // ignore
        skipIp.ranges_.emplace_back("200.100.10.12", "220.140.20.38"); // ignore
        skipIp.ranges_.emplace_back("240.10.20.41", "240.10.20.50"); // ignore

        skipIp.elements_.emplace_back("50.10.20.29"); // ignore
        skipIp.elements_.emplace_back("192.140.20.38"); // ignore
        skipIp.elements_.emplace_back("200.100.10.12"); // ignore
        skipIp.elements_.emplace_back("220.140.20.39"); // ignore
        skipIp.elements_.emplace_back("240.10.20.41"); // ignore


        originalRanges.ranges_ =
        {
            { "50.10.20.30", "60.30.15.25" },
            { "192.140.20.40", "200.100.10.10" },
            { "220.140.20.40", "240.10.20.40" },
        };

        originalRanges.elements_ = { "190.10.23.44", "192.140.20.39", "200.100.10.11"};

        fillStringRangesToIpDataRanges(stringRanges, skipIp, ranges);
        compareOriginalToResults(originalRanges, ranges);
    }
};

class FillRangesTest7 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("50.10.20.30", "60.30.15.25");
        stringRanges.ranges_.emplace_back("192.140.20.40", "200.100.10.10");
        stringRanges.ranges_.emplace_back("220.140.20.40", "240.10.20.40");
        // split  ["12.10.20.20", "12.25.10.30"]
        // split ["13.10.200.10"]
        stringRanges.ranges_.emplace_back("10.15.20.40", "15.100.20.40"); 

        stringRanges.elements_.emplace_back("190.10.23.44"); // ok
        stringRanges.elements_.emplace_back("192.140.20.39"); // ok
        stringRanges.elements_.emplace_back("200.100.10.11"); // ok
        stringRanges.elements_.emplace_back("60.30.15.25"); // ignore
        stringRanges.elements_.emplace_back("240.10.20.40"); // ignore
        stringRanges.elements_.emplace_back("50.10.20.30"); // skip  by ["50.10.20.28", "58.25.20.29"]
        stringRanges.elements_.emplace_back("201.10.20.30"); // skip by ["200.100.10.12", "220.140.20.38"]

        ElementRanges<IpData> skipIp;
        skipIp.ranges_.emplace_back("50.10.20.28", "50.10.20.29"); // ignore
        skipIp.ranges_.emplace_back("60.30.15.26", "60.30.15.26"); // ignore
        skipIp.ranges_.emplace_back("200.100.10.12", "220.140.20.38"); // ignore
        skipIp.ranges_.emplace_back("240.10.20.41", "240.10.20.50"); // ignore
        skipIp.ranges_.emplace_back("50.10.20.28", "58.25.20.29"); // ok
        skipIp.ranges_.emplace_back("12.10.20.20", "12.25.10.30"); // ok

        skipIp.elements_.emplace_back("50.10.20.29"); // ignore
        skipIp.elements_.emplace_back("192.140.20.38"); // ignore
        skipIp.elements_.emplace_back("200.100.10.12"); // ignore
        skipIp.elements_.emplace_back("220.140.20.39"); // ignore
        skipIp.elements_.emplace_back("240.10.20.41"); // ignore
        skipIp.elements_.emplace_back("13.10.200.10"); // ok

        ElementRanges<std::string> originalRanges;
        originalRanges.ranges_ =
        {
            { "10.15.20.40", "12.10.20.19" },
            { "12.25.10.31", "13.10.200.9" },
            { "13.10.200.11", "15.100.20.40" },
            { "58.25.20.30", "60.30.15.25"},
            { "192.140.20.40", "200.100.10.10" },
            { "220.140.20.40", "240.10.20.40" },
        };

        originalRanges.elements_ = { "190.10.23.44", "192.140.20.39", "200.100.10.11" };

        ElementRanges<IpData> ranges;
        fillStringRangesToIpDataRanges(stringRanges, skipIp, ranges);
        compareOriginalToResults(originalRanges, ranges);
    }
};

class FillRangesTest8 : public IFillRangesTest
{
protected:
    void run() override
    {
        stringRanges.ranges_.emplace_back("192.168.1.20", "192.168.10.252");

        ElementRanges<IpData> ipRanges;
        ElementRanges<IpData> skipRanges;

        skipRanges.ranges_.emplace_back("192.168.1.255", "192.168.9.255");
        skipRanges.elements_.emplace_back("192.168.1.252");
        skipRanges.elements_.emplace_back("192.168.10.252");

        fillStringRangesToIpDataRanges(stringRanges, skipRanges, ipRanges);

        ElementRanges<std::string> originalRanges;

        originalRanges.ranges_.emplace_back("192.168.1.20", "192.168.1.251");
        originalRanges.ranges_.emplace_back("192.168.1.253", "192.168.1.254");
        originalRanges.ranges_.emplace_back("192.168.10.0", "192.168.10.251");

        compareOriginalToResults(originalRanges, ipRanges);
    }
};

void addFillRangesTest()
{
    new FillRangesTest1;
    new FillRangesTest2;
    new FillRangesTest3;
    new FillRangesTest4;
    new FillRangesTest5;
    new FillRangesTest6;
    new FillRangesTest7;
    new FillRangesTest8;
}