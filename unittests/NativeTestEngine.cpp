#include "NativeTestEngine.h"
#include <stdarg.h>
#include  <stdexcept>
#include "typeinfo" // for unittest we can use RTTI


INativeTest::INativeTest()
{
    NativeTests::self().AddTest(*this);
}

INativeTest::~INativeTest()
{

}

void INativeTest::internal_run()
{
    testName_ = typeid(*this).name();

    try
    {
        run();
        printf("[%s]\tSUCCESS\n", testName_.c_str());
    }
    catch (std::exception&)
    {

    }
}

void INativeTest::assertTrue(bool val, const char* format, ...)
{
    if (val) {
        return;
    }

    va_list args;
    va_start(args, format);
    size_t written = vsnprintf(assertTraceBuffer_, sizeof(assertTraceBuffer_), format, args);
    va_end(args);

    printf("[%s]\tFAILURE\t %.*s \n ", testName_.c_str(), (unsigned int)written, assertTraceBuffer_);
    throw std::runtime_error("Failure");
}

NativeTests& NativeTests::self()
{
    static NativeTests me;
    return me;
}

void NativeTests::run()
{
    for (auto& r : tests_) {
        r->internal_run();
    }
}

void NativeTests::deinit()
{
    for (INativeTest* r : tests_) {
        delete r;
    }

    tests_.clear();
}

void NativeTests::AddTest(INativeTest& test)
{
    tests_.insert(&test);
}
