#pragma once

#include <include.h>

namespace RakNet {

class OpenConnectionReply1Packet {
public:
    int64_t serverGUID;
    bool serverHasSecurity;
    uint32_t cookie;
    uint16_t mtu;

    std::optional<std::string> decode(std::span<uint8_t> buffer);
    [[nodiscard]] std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> encode() const;
};

}
