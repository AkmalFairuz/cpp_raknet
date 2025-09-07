#pragma once

#include "UdpEndpoint.h"
#include "include.h"

namespace RakNet {
    
    class Connection {
    public:
        std::optional<std::string> write(std::span<uint8_t> buffer) const;
    private:
        UdpEndpoint endpoint;
    };
}
