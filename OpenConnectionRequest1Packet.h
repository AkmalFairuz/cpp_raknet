#pragma once

#include <include.h>

namespace RakNet {

class OpenConnectionRequest1Packet {
public:
    uint8_t clientProtocol;
    uint16_t mtu;

    std::optional<std::string> decode(std::span<uint8_t> buffer);
    [[nodiscard]] std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> encode() const;
};

}
