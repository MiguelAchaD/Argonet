#pragma once

#include <string>

namespace proxyServer {
class Accepter {
    private:
        std::string translateContents();
        void accept();
        void reject();
    protected:
        void examineContents();
    public:
        Accepter();
        ~Accepter();
};
}
