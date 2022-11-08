#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <vector>

namespace Log
{
    typedef std::pair<bool, bool> DateTimeNeedTrace;

    class LogTracer
    {
    public:
        explicit LogTracer(bool useMutex);
        ~LogTracer();

        bool open(const std::string& fileName, unsigned short logTypeMask, unsigned short maxTraceLineSize, DateTimeNeedTrace dateTimeNeedTrace);

        bool close();

        bool isOpen() const;

        void setLogTypeMask(unsigned short logTypeMask);

        void trace(unsigned short logTypeMask, const char* format, ...);

        unsigned short getLogMask() const;

    private:
        std::unique_ptr<std::lock_guard<std::mutex>> createGuard() const;
        bool close_i();

    private:
        bool useMutex_ = false;
        std::string fileName_;
        std::atomic<unsigned short> logTypeMask_ = ATOMIC_VAR_INIT(0);
        unsigned short maxTraceLineSize_ = 1024;
        DateTimeNeedTrace dateTimeNeedTrace_;

        std::ofstream fileTracer_;

        mutable std::mutex mutex_;

        std::atomic_bool isOpen_ = ATOMIC_VAR_INIT(0);
        std::unique_ptr<char[]> outBuffer_;
    };

}

#define LOG_CHECK_MASK_IF(tracer, mask) ((tracer.getLogMask() & mask) != 0)
#define LOG_TRACER(tracer, mask, desc, ...) if(LOG_CHECK_MASK_IF(tracer, mask)) { tracer.trace(mask, desc "\t__"  __FUNCTION__ "__\t" __VA_ARGS__); }