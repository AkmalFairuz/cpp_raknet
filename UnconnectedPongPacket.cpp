#include "UnconnectedPongPacket.h"

std::optional<std::string> RakNet::UnconnectedPongPacket::decode(std::span<uint8_t> buffer) {
    if (buffer.size() < 34) {
        return std::string("Packet too short");
    }
    fromBytesBE(&this->pingTime, buffer.data());
    fromBytesBE(&this->serverGUID, buffer.data() + 8);
    // Magic 16 bytes
    uint16_t n;
    fromBytesBE(&n, buffer.data() + 32);
    if (buffer.size() < 34 + n) {
        return std::string(std::format("Packet too short, need {}, have {} bytes", buffer.size() - 34, n));
    }
    this->data = std::string(buffer.data() + 34, buffer.data() + 34 + n);
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::UnconnectedPongPacket::encode() const {
    const uint16_t n = this->data.length();
    auto buffer = std::make_unique<std::vector<uint8_t>>(35+n);
    buffer->at(0) = ID_UNCONNECTED_PONG;
    toBytesBE(this->pingTime, buffer->data() + 1);
    toBytesBE(this->serverGUID, buffer->data() + 9);
    std::ranges::copy(unconnectedMessageSequence, buffer->data() + 17);
    toBytesBE(n, buffer->data() + 33);
    std::ranges::copy(this->data, buffer->data() + 35);
    return buffer;
}