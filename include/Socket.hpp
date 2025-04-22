#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <atomic>

namespace proxyServer {
<<<<<<< Updated upstream
class Socket {
=======
<<<<<<< Updated upstream
    class Socket {
=======
class Socket {
>>>>>>> Stashed changes
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
        enum class SocketType {
            ACCEPTER,
            FORWARDER,
            SENDER
        };
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes
        Socket(unsigned short int t_port_number);
        virtual ~Socket();

        bool initialiseSocket();
        void configureListener();
        void startListener(ExecutionType t_execution_type);

        bool removeSocket();
<<<<<<< Updated upstream
};
=======
<<<<<<< Updated upstream
    };
=======
};
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}

