#include "LogTracer.h"
#include <stdarg.h> 
#include <chrono>

using sysclock_t = std::chrono::system_clock;

namespace Log
{
    LogTracer::LogTracer(bool useMutexForTrace)
        : useMutex_(useMutexForTrace)
    {
    }

    LogTracer::~LogTracer()
    {
        close();
    }

    bool LogTracer::open(const std::string& fileName, unsigned short logTypeMask, unsigned short maxTraceLineSize, DateTimeNeedTrace dateTimeNeedTrace)
    {
        auto guard = createGuard();
        if (isOpen_) {
            return false;
        }

        fileName_ = fileName;
        setLogTypeMask(logTypeMask);
        maxTraceLineSize_ = std::max<unsigned short>(128, maxTraceLineSize);
        dateTimeNeedTrace_ = dateTimeNeedTrace;

        outBuffer_ = std::make_unique<char[]>(maxTraceLineSize_);
        
        fileTracer_.open(fileName);
        isOpen_ = (fileTracer_.is_open());

        return true;
    }

    bool LogTracer::isOpen() const
    {
        return isOpen_;
    }

    void LogTracer::setLogTypeMask(unsigned short logTypeMask)
    {
        logTypeMask_ = logTypeMask;
    }

    void LogTracer::trace(unsigned short logTypeMask, const char* format, ...)
    {
        auto canTrace = [&]() -> bool
        {
            if ((logTypeMask_ & logTypeMask) == 0) {
                return false;
            }

            if (!isOpen()) {
                return false;
            }

            return true;
        };

        if (!canTrace()) {
            return;
        }

        auto guard = createGuard();

        if (guard) {
            // double check 
            if (!canTrace()) {
                return;
            }
        }

        char* pWriteBuffer = outBuffer_.get();
        
        // need to trace time
        if (dateTimeNeedTrace_.first || dateTimeNeedTrace_.second) {
            std::time_t now = sysclock_t::to_time_t(sysclock_t::now());
            struct tm localTime;
            // time_t now = time(0);
            localtime_s(&localTime, &now);

            if (dateTimeNeedTrace_.first) {
                size_t res = std::strftime(pWriteBuffer, 26, "%Y-%m-%d ", &localTime);
                pWriteBuffer += res;
            }

            if (dateTimeNeedTrace_.second) {
                size_t res = std::strftime(pWriteBuffer, 26, "%H:%M:%S ", &localTime);
                pWriteBuffer += res;
            }
        }

        size_t writen = pWriteBuffer - outBuffer_.get();

        va_list args;
        va_start(args, format);
        writen += vsnprintf(pWriteBuffer, maxTraceLineSize_ - writen, format, args);
        va_end(args);

        fileTracer_.write(outBuffer_.get(), writen);
        fileTracer_.flush();
    }

    unsigned short LogTracer::getLogMask() const
    {
        return logTypeMask_;
    }

    std::unique_ptr<std::lock_guard<std::mutex>> LogTracer::createGuard() const
    {
        std::unique_ptr<std::lock_guard<std::mutex>> guard;

        if (useMutex_) {
            guard = std::make_unique<std::lock_guard<std::mutex>>(mutex_);
        }

        return std::move(guard);
    }

    bool LogTracer::close_i()
    {
        if (!isOpen_) {
            return false;
        }

        isOpen_ = false;

        fileTracer_.close();

        return true;
    }

    bool LogTracer::close()
    {
        auto guard = createGuard();
        bool status = close_i();
        return status;
    }

}
