#include "UnconnectedPingPacket.h"

std::optional<std::string> RakNet::UnconnectedPingPacket::decode(std::span<uint8_t> buffer) {
    if (buffer.size() < 32) {
        return std::string("Packet too short");
    }
    this->pingTime = FROM_BE(int64_t, buffer.data());
    // Magic 16 bytes
    this->clientGUID = FROM_BE(int64_t, buffer.data() + 24);
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::UnconnectedPingPacket::encode() const {
    auto buffer = std::make_unique<std::vector<uint8_t>>(33);
    buffer->at(0) = ID_UNCONNECTED_PING;
    TO_BE(int64_t, this->pingTime, buffer->data()+1);
    std::ranges::copy(unconnectedMessageSequence, buffer->data() + 9);
    TO_BE(int64_t, this->clientGUID, buffer->data() + 25);
    return buffer;
}
