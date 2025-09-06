#include "OpenConnectionReply2Packet.h"

std::optional<std::string> RakNet::OpenConnectionReply2Packet::decode(Buffer buffer) {
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::OpenConnectionReply2Packet::encode() const {
    auto buffer = std::make_unique<std::vector<uint8_t>>(33);
    return buffer;
}