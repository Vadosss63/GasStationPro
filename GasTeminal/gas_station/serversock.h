#ifndef SERVERSOCK_H
#define SERVERSOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string>
#include <array>
#include <functional>
#include <atomic>

class ServerSock
{
public:
    static constexpr int sizeInBuff = 64;
    static constexpr int sizeOutBuff = 512;

    ServerSock(std::function<std::array<char, sizeOutBuff>(std::array<char, sizeInBuff>&)> _callback);
    ~ServerSock();

    void startServerSock();

    void stopServer();

private:
    const char* socket_path = "/tmp/t_cash.sock";
    static constexpr unsigned int nIncomingConnections = 5;
    std::atomic<bool> isRunning = false;
    std::array<char, sizeInBuff> recv_buf{};

    std::function<std::array<char, sizeOutBuff>(std::array<char, sizeInBuff>&)> callback;
    int socketFileDescriptor = 0;
    int socketDescriptor = 0;

};

#endif // SERVERSOCK_H
