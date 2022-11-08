#ifdef _WIN32
#include "Windows.h"
#endif

#include "IpData.h"
#include "Networking.h"

namespace nw
{
    struct EngineInit
    {
        EngineInit()
        {
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        }

        ~EngineInit()
        {
#ifdef _WIN32
            WSACleanup();
#endif
        }
    };


    Client::Client(const Options& opts) 
        : opts_(opts)
    {
        static EngineInit engineInit;
    }

    Client::~Client()
    {
        disconnect();
    }

    bool Client::connect(const IpData& ipData, ub2 port)
    {
#ifdef _WIN32
        socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        
        if (socket_ == INVALID_SOCKET) {
            return false;
        }

        struct sockaddr_in serverAddrIn;
        serverAddrIn.sin_addr.s_addr = ipData.toInetAddr();
        serverAddrIn.sin_family = AF_INET;
        serverAddrIn.sin_port = htons(port);

        auto connectByTimeout = [](const Options& opts, NWSOCKET socket, struct sockaddr_in& serverAddrIn)
        {
            int outError = -1;
            int socketOptLen = sizeof(int);
            timeval connTimeVal;
            fd_set descriptorSet;
            unsigned long socketOptUl = 1;
            ioctlsocket(socket, FIONBIO, &socketOptUl); //set as non-blocking
            bool status = false;
            if (::connect(socket, (struct sockaddr*)&serverAddrIn, sizeof(serverAddrIn)) == -1)
            {
                connTimeVal.tv_sec = opts.connectingTimeout_.first;
                connTimeVal.tv_usec = opts.connectingTimeout_.second;

                FD_ZERO(&descriptorSet);
                FD_SET(socket, &descriptorSet);

                if (select((int)socket + 1, NULL, &descriptorSet, NULL, &connTimeVal) > 0)
                {
                    getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)&outError, /*(socklen_t *)*/&socketOptLen);
                    if (outError == 0) {
                        status = true;
                    }
                    else {
                        status = false;
                    }
                }
                else {
                    status = false;
                }
            }
            else {
                status = true;
            }

            socketOptUl = 0;
            ioctlsocket(socket, FIONBIO, &socketOptUl); //set as blocking
            if (!status)
            {
                closesocket(socket);
                return false;
            }

            return true;
        };

        if (!connectByTimeout(opts_, socket_, serverAddrIn)) {
            socket_ = INVALID_SOCKET;
            return false;
        }

        DWORD ioTimeout = opts_.ioTimeout_;

        if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char*)&ioTimeout, sizeof(DWORD)))
        {
            disconnect();
            return false;
        }

        ioTimeout = opts_.ioTimeout_;

        if (setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (char*)&ioTimeout, sizeof(DWORD)))
        {
            disconnect();
            return false;
        }

        char nodelayEnable = 1;
        if (setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, &nodelayEnable, sizeof(nodelayEnable))) {
            disconnect();
            return false;
        }

#endif

        return true;

    }

    void Client::disconnect()
    {
#ifdef _WIN32
        if (socket_ == INVALID_SOCKET) {
            return ;
        }

        shutdown(socket_, 2);
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
#endif
    }

    bool Client::sendPacket(const char* data, int dataSize)
    {
#ifdef _WIN32
        // when blocking mode
        return ::send(socket_, data, dataSize, 0) > 0;
#endif
    }

    bool Client::recvPacket(char* data, int dataSize)
    {
#ifdef _WIN32
        return ::recv(socket_, data, dataSize, 0) == dataSize;
#endif
    }

}