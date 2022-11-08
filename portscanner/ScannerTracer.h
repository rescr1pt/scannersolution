#pragma once

#include "LogTracer.h"

enum class LogTypeMask : unsigned short
{
    Info = 0x1,
    Error = 0x02,
};

class ScannerTracer : public Log::LogTracer
{
public:
    ScannerTracer();
    static ScannerTracer& self();
};

#define SCANNER_CHECK_MASK_IF(mask) LOG_CHECK_MASK_IF(ScannerTracer::self(), (unsigned short)mask)
#define SCANNER_TRACE_ERROR(...) LOG_TRACER(ScannerTracer::self(), (unsigned short)LogTypeMask::Error, "ERROR", __VA_ARGS__);
#define SCANNER_TRACE_INFO(...) LOG_TRACER(ScannerTracer::self(), (unsigned short)LogTypeMask::Info, "INFO", __VA_ARGS__);
