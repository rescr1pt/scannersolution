#pragma once

#include "Types.h"

enum class ScanExecutorExecutedStage : ub2
{
    None = 0, // failed
    Connected = 1, // connected to server
    ProtocolChecked = 2, //  protocol is valid 
};
