#pragma once

#include <include.h>

namespace RakNet {

class OpenConnectionReply2Packet {
public:
    int64_t serverGUID;
    UdpEndpoint clientAddress;
    uint16_t mtu;
    bool doSecurity;

    std::optional<std::string> decode(Buffer buffer);
    [[nodiscard]] std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> encode() const;
};

}
