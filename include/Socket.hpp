#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <atomic>

namespace proxyServer {
class Socket {
protected:
    int socket_fd;
    struct sockaddr_in address;
    unsigned short int port_number;
    std::atomic<bool> running;

        bool createSocket(int socket_type = SOCK_STREAM);
        void closeSocket();
        bool setSocketTimeout(int seconds);

    public:
        enum class ExecutionType {
            DEFERRED,
            ASYNC
        };
        enum class SocketType {
            ACCEPTER,
            FORWARDER,
            SENDER
        };
        Socket(unsigned short int t_port_number);
        virtual ~Socket();
};
}

