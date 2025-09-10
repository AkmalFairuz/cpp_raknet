#include "UnconnectedPingPacket.h"

std::optional<std::string> RakNet::UnconnectedPingPacket::decode(std::span<uint8_t> buffer) {
    if (buffer.size() < 32) {
        return std::string("Packet too short");
    }
    fromBytesBE(&this->pingTime, buffer.data());
    // Magic 16 bytes
    fromBytesBE(&this->clientGUID, buffer.data() + 24);
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::UnconnectedPingPacket::encode() const {
    auto buffer = std::make_unique<std::vector<uint8_t>>(33);
    buffer->at(0) = ID_UNCONNECTED_PING;
    toBytesBE(this->pingTime, buffer->data()+1);
    std::ranges::copy(unconnectedMessageSequence, buffer->data() + 9);
    toBytesBE(this->clientGUID, buffer->data() + 25);
    return buffer;
}
