#pragma once
#include <netinet/in.h>
#include <functional>

namespace RakNet {

struct UdpEndpoint {
    in_addr addr;
    uint16_t port;

    bool operator == (const UdpEndpoint& other) const {
        return addr.s_addr == other.addr.s_addr && port == other.port;
    }
};

struct UdpEndpointHash {
    std::size_t operator()(const UdpEndpoint& ep) const noexcept {
        const std::size_t h1 = std::hash<uint32_t>{}(ep.addr.s_addr);
        const std::size_t h2 = std::hash<uint16_t>{}(ep.port);
        return h1 ^ (h2 << 1);
    }
};

UdpEndpoint toUdpEndpoint(const sockaddr_in& sa);
sockaddr_in fromUdpEndpoint(const UdpEndpoint& ep);

}