#pragma once
#include <include.h>

namespace RakNet {

class UnconnectedPongPacket {
    public:
        int64_t pingTime;
        int64_t serverGUID;
        std::string data;
        std::optional<std::string> decode(Buffer buffer);
        [[nodiscard]] std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> encode() const;
};

}