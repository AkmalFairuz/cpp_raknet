#include <UdpEndpoint.h>

namespace RakNet {

UdpEndpoint toUdpEndpoint(const sockaddr_in& sa) {
    UdpEndpoint ep{};
    ep.addr = sa.sin_addr;
    ep.port = ntohs(sa.sin_port);
    return ep;
}

sockaddr_in fromUdpEndpoint(const UdpEndpoint& ep) {
    sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(ep.port);
    sa.sin_addr = ep.addr;
    return sa;
}

}