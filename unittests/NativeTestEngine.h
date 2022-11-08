#pragma once

#include <set>
#include <string>

class INativeTest
{
    friend class NativeTests;

protected:
    INativeTest();
    virtual ~INativeTest();

    virtual void run() = 0;
    void internal_run();

    void assertTrue(bool val, const char* format, ...);

private:
    std::string testName_;
    char assertTraceBuffer_[4096] = { 0 };
};

class NativeTests
{
    friend class INativeTest;
public:
    static NativeTests& self();

    void run();
    void deinit();

private:
    NativeTests() = default;
    NativeTests(NativeTests const&) = default;
    ~NativeTests() {}

    void AddTest(INativeTest& test);

private:
    std::set<INativeTest*> tests_;
};