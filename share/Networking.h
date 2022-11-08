#pragma once

#include <string>
#include "Types.h"

#ifdef _WIN32 
#include <winsock.h>
#define NWSOCKET SOCKET
#endif

struct IpData;

namespace nw
{
    struct Options
    {
        typedef std::pair<ub4, ub4> TimeVal;
        TimeVal connectingTimeout_ = { 1, 0 }; // sec, microsecs
        ub4 ioTimeout_ = 1000; // milliseconds
    };

    class Client
    {
    public:
        Client(const Options& opts);
        ~Client();

        bool connect(const IpData& ipData , ub2 port);
        void disconnect();

        bool sendPacket(const char* data, int dataSize);
        bool recvPacket(char* data, int dataSize);


    private:
        const Options& opts_;
        NWSOCKET socket_ = 0;
    };
}