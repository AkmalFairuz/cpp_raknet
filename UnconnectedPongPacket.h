#pragma once
#include <include.h>
#include "Buffer.h"

namespace RakNet {

class UnconnectedPongPacket {
    public:
        int64_t pingTime;
        int64_t serverGUID;
        std::string data;
        std::optional<std::string> decode(std::span<uint8_t> buffer);
        [[nodiscard]] std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> encode() const;
};

}