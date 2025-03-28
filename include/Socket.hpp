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
        std::thread listen_thread;
        std::atomic<bool> running;
        std::atomic<bool> active;

        bool bindSocket();
        bool listenSocket();
        virtual void listenLoop() = 0;

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

        bool initialiseSocket();
        void configureListener();
        void startListener(ExecutionType t_execution_type);

        bool removeSocket();
};
}

