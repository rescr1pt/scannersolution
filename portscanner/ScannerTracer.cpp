#include "ScannerTracer.h"

ScannerTracer::ScannerTracer() 
    : LogTracer(true)
{

}

ScannerTracer& ScannerTracer::self()
{
    static ScannerTracer me;
    return me;
}

