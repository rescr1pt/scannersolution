#include "NativeTestEngine.h"
#include "RangeElementTools.h"
#include "IpData.h"

class IRangesNextTest : protected INativeTest
{
protected:
    static ub2& portToDecimal(const ub2& val)
    {
        // selftype is nativeType, use const cast
        return const_cast<ub2&>(val);
    };

    static ub4& hostToDecimal(IpData& val)
    {
        // self type is nativeType
        return val.data_.dec_;
    };
};

class RangesNextTest1 : public IRangesNextTest
{
protected:
    void run() override
    {
        {
            ElementRanges<ub2> portRanges;

            PortRangeNext portRangeNext(portRanges);
            assertTrue(portRangeNext.isEof(), "");
        }

        {
            ElementRanges<ub2> portRanges;
            portRanges.elements_.emplace_back(5);

            PortRangeNext portRangeNext(portRanges);
            assertTrue(!portRangeNext.isEof(), "");
            assertTrue(portRangeNext.next() == 5, "");
            assertTrue(portRangeNext.isEof(), "");
        }

        {
            ElementRanges<ub2> portRanges;
            portRanges.ranges_.emplace_back(5, 7);

            PortRangeNext portRangeNext(portRanges);
            assertTrue(!portRangeNext.isEof(), "");
            assertTrue(portRangeNext.next() == 5, "");
            assertTrue(portRangeNext.next() == 6, "");
            assertTrue(portRangeNext.next() == 7, "");
            assertTrue(portRangeNext.isEof(), "");
        }

        {
            ElementRanges<ub2> portRanges;
            portRanges.elements_.emplace_back(5);
            portRanges.ranges_.emplace_back(6, 7);

            PortRangeNext portRangeNext(portRanges);
            assertTrue(!portRangeNext.isEof(), "");
            assertTrue(portRangeNext.next() == 5, "");
            assertTrue(portRangeNext.next() == 6, "");
            assertTrue(portRangeNext.next() == 7, "");
            assertTrue(portRangeNext.isEof(), "");
        }
    }
};

class RangesNextTest2 : public IRangesNextTest
{
protected:
    void run() override
    {
        ElementRanges<ub2> portRanges;
        portRanges.ranges_.emplace_back(5, 5);
        portRanges.ranges_.emplace_back(7, 8);
        portRanges.ranges_.emplace_back(10, 20);
        portRanges.ranges_.emplace_back(30, 40);
        portRanges.ranges_.emplace_back(50, 70);

        PortRangeNext portRangeNext(portRanges);
        assertTrue(!portRangeNext.isEof(), "");

        assertTrue(portRangeNext.next() == 5, "");
        assertTrue(portRangeNext.next() == 7, "");
        assertTrue(portRangeNext.next() == 8, "");

        for (ub2 i = 10; i <= 20; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        for (ub2 i = 30; i <= 40; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        for (ub2 i = 50; i <= 70; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.isEof(), "");
    }
};

class RangesNextTest3 : public IRangesNextTest
{
protected:
    void run() override
    {
        ElementRanges<ub2> portRanges;
        portRanges.elements_.emplace_back(0);
        portRanges.elements_.emplace_back(5);
        portRanges.elements_.emplace_back(10);
        portRanges.elements_.emplace_back(20);

        PortRangeNext portRangeNext(portRanges);

        assertTrue(!portRangeNext.isEof(), "");

        assertTrue(portRangeNext.next() == 0, "");
        assertTrue(portRangeNext.next() == 5, "");
        assertTrue(portRangeNext.next() == 10, "");
        assertTrue(portRangeNext.next() == 20, "");

        assertTrue(portRangeNext.isEof(), "");
    }
};

class RangesNextTest4 : public IRangesNextTest
{
protected:
    void run() override
    {
        ElementRanges<ub2> portRanges;
        portRanges.elements_.emplace_back(0);
        portRanges.elements_.emplace_back(5);
        portRanges.elements_.emplace_back(10);

        portRanges.ranges_.emplace_back(10, 14);
        portRanges.elements_.emplace_back(15);
        portRanges.ranges_.emplace_back(16, 20);
        portRanges.ranges_.emplace_back(20, 22);
        portRanges.elements_.emplace_back(30);
        portRanges.elements_.emplace_back(32);
        portRanges.elements_.emplace_back(34);
        portRanges.ranges_.emplace_back(35, 38);


        PortRangeNext portRangeNext(portRanges);

        assertTrue(!portRangeNext.isEof(), "");
        assertTrue(portRangeNext.next() == 0, "");
        assertTrue(portRangeNext.next() == 5, "");

        for (ub2 i = 10; i <= 14; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.next() == 15, "");

        for (ub2 i = 16; i <= 20; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        for (ub2 i = 20; i <= 22; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.next() == 30, "");
        assertTrue(portRangeNext.next() == 32, "");
        assertTrue(portRangeNext.next() == 34, "");

        for (ub2 i = 35; i <= 38; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.isEof(), "");
    }
};

class RangesNextTest5 : public IRangesNextTest
{
protected:
    void run() override
    {
        ElementRanges<ub2> portRanges;
        portRanges.ranges_.emplace_back(0, 10);
        portRanges.ranges_.emplace_back(10, 14);
        portRanges.elements_.emplace_back(15);
        portRanges.ranges_.emplace_back(16, 20);
        portRanges.ranges_.emplace_back(20, 22);
        portRanges.elements_.emplace_back(30);
        portRanges.elements_.emplace_back(32);
        portRanges.elements_.emplace_back(34);
        portRanges.ranges_.emplace_back(35, 38);
        portRanges.elements_.emplace_back(39);
        portRanges.elements_.emplace_back(41);

        PortRangeNext portRangeNext(portRanges);

        for (ub2 i = 0; i <= 10; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        for (ub2 i = 10; i <= 14; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.next() == 15, "");

        for (ub2 i = 16; i <= 20; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        for (ub2 i = 20; i <= 22; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.next() == 30, "");
        assertTrue(portRangeNext.next() == 32, "");
        assertTrue(portRangeNext.next() == 34, "");

        for (ub2 i = 35; i <= 38; ++i) {
            assertTrue(portRangeNext.next() == i, "");
        }

        assertTrue(portRangeNext.next() == 39, "");
        assertTrue(portRangeNext.next() == 41, "");

        assertTrue(portRangeNext.isEof(), "");
    }
};

class RangesNextTest6 : public IRangesNextTest
{
protected:
    void run() override
    {
        ElementRanges<IpData> ipRanges;
        ipRanges.elements_.emplace_back("192.168.1.10");
        ipRanges.elements_.emplace_back("192.168.1.11");
        ipRanges.ranges_.emplace_back("192.168.1.12", "192.168.1.15");
        ipRanges.elements_.emplace_back("192.168.1.16");
        ipRanges.ranges_.emplace_back("192.168.10.253", "192.168.11.4");

        IpDataRangeNext ipRangeNext(ipRanges);
        assertTrue(!ipRangeNext.isEof(), "");

        for (size_t i = 10; i <= 16; ++i) {
            std::string src = "192.168.1." + std::to_string(i);
            assertTrue(ipRangeNext.next() == src, "");
        }

        assertTrue(ipRangeNext.next() == "192.168.10.253", "");
        assertTrue(ipRangeNext.next() == "192.168.10.254", "");
        assertTrue(ipRangeNext.next() == "192.168.10.255", "");
        assertTrue(ipRangeNext.next() == "192.168.11.0", "");
        assertTrue(ipRangeNext.next() == "192.168.11.1", "");
        assertTrue(ipRangeNext.next() == "192.168.11.2", "");
        assertTrue(ipRangeNext.next() == "192.168.11.3", "");
        assertTrue(ipRangeNext.next() == "192.168.11.4", "");

        assertTrue(ipRangeNext.isEof(), "");
    }
};

class RangesNextTest7 : public IRangesNextTest
{
protected:
    void run() override
    {
        {
            ElementRanges<ub2> portRanges;

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(4);
            portRangeNext.skipBefore(6);
            portRangeNext.skipBefore(0);
            portRangeNext.skipBefore(1);
            assertTrue(portRangeNext.isEof(), "");

        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.elements_.emplace_back(5);
            portRanges.elements_.emplace_back(7);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(5);
            portRangeNext.skipBefore(7);
            assertTrue(!portRangeNext.isEof(), "");
            assertTrue(portRangeNext.next() == 7, "");
            assertTrue(portRangeNext.isEof(), "");

        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.elements_.emplace_back(5);
            portRanges.elements_.emplace_back(7);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(4);
            portRangeNext.skipBefore(6);
            portRangeNext.skipBefore(0);
            portRangeNext.skipBefore(1);
            assertTrue(!portRangeNext.isEof(), "");
            assertTrue(portRangeNext.next() == 7, "");
            assertTrue(portRangeNext.isEof(), "");

        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.elements_.emplace_back(5);
            portRanges.elements_.emplace_back(7);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(8);
            assertTrue(portRangeNext.isEof(), "");

        }
    }
};

class RangesNextTest8 : public IRangesNextTest
{
protected:
    void run() override
    {
        {
            ElementRanges<ub2> portRanges;

            portRanges.ranges_.emplace_back(10, 13);
            portRanges.ranges_.emplace_back(14, 16);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(10);
            for (ub2 i = 10; i <= 16; ++i) {
                assertTrue(portRangeNext.next() == i, "");

            }
            assertTrue(portRangeNext.isEof(), "");

        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.ranges_.emplace_back(10, 13);
            portRanges.ranges_.emplace_back(14, 16);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(11);
            portRangeNext.skipBefore(3);
            portRangeNext.skipBefore(11);
            for (ub2 i = 11; i <= 16; ++i) {
                assertTrue(portRangeNext.next() == i, "");

            }
            assertTrue(portRangeNext.isEof(), "");

        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.elements_.emplace_back(5);
            portRanges.elements_.emplace_back(10);
            portRanges.elements_.emplace_back(21);
            portRanges.ranges_.emplace_back(10, 13);
            portRanges.ranges_.emplace_back(14, 16);
            portRanges.ranges_.emplace_back(17, 20);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(11);
            portRangeNext.skipBefore(3);
            portRangeNext.skipBefore(11);
            for (ub2 i = 11; i <= 21; ++i) {
                assertTrue(portRangeNext.next() == i, "");

            }
            assertTrue(portRangeNext.isEof(), "");
        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.elements_.emplace_back(5);
            portRanges.elements_.emplace_back(10);
            portRanges.elements_.emplace_back(21);
            portRanges.ranges_.emplace_back(10, 13);
            portRanges.ranges_.emplace_back(14, 16);
            portRanges.ranges_.emplace_back(17, 20);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(22);
            assertTrue(portRangeNext.isEof(), "");
        }

        {
            ElementRanges<ub2> portRanges;

            portRanges.elements_.emplace_back(5);
            portRanges.elements_.emplace_back(10);
            portRanges.elements_.emplace_back(21);
            portRanges.ranges_.emplace_back(10, 13);
            portRanges.ranges_.emplace_back(14, 16);
            portRanges.ranges_.emplace_back(17, 20);

            PortRangeNext portRangeNext(portRanges);
            portRangeNext.skipBefore(3);
            assertTrue(portRangeNext.next() == 5, "");
            portRangeNext.skipBefore(10);
            assertTrue(portRangeNext.next() == 10, "");
            portRangeNext.skipBefore(11);
            assertTrue(portRangeNext.next() == 11, "");
            portRangeNext.skipBefore(19);
            assertTrue(portRangeNext.next() == 19, "");
            portRangeNext.skipBefore(20);
            assertTrue(portRangeNext.next() == 20, "");
            portRangeNext.skipBefore(20);
            assertTrue(portRangeNext.next() == 21, "");
            assertTrue(portRangeNext.isEof(), "");

        }
    }
};

class RangesNextTest9 : public IRangesNextTest
{
protected:
    void run() override
    {
        {
            ElementRanges<IpData> hostRanges;
            hostRanges.elements_.emplace_back("192.168.1.10");
            hostRanges.elements_.emplace_back("192.168.1.12");
            hostRanges.elements_.emplace_back("192.168.1.14");

            hostRanges.ranges_.emplace_back("192.168.1.20", "192.168.1.22");
            hostRanges.ranges_.emplace_back("192.168.1.24", "192.168.1.26");

            IpDataRangeNext ipRangeNext(hostRanges);
            ipRangeNext.skipBefore("192.168.1.23");
            assertTrue(!ipRangeNext.isEof(), "");
            assertTrue(ipRangeNext.next() == "192.168.1.24", "");
            assertTrue(ipRangeNext.next() == "192.168.1.25", "");
            assertTrue(ipRangeNext.next() == "192.168.1.26", "");
            assertTrue(ipRangeNext.isEof(), "");
        }
    }
};

void addRangesNextTest()
{
    new RangesNextTest1;
    new RangesNextTest2;
    new RangesNextTest3;
    new RangesNextTest4;
    new RangesNextTest5;
    new RangesNextTest6;
    new RangesNextTest7;
    new RangesNextTest8;
    new RangesNextTest9;
}