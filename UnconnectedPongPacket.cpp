#include "UnconnectedPongPacket.h"

std::optional<std::string> RakNet::UnconnectedPongPacket::decode(Buffer buffer) {
    if (buffer.length < 34) {
        return std::string("Packet too short");
    }
    this->pingTime = FROM_BE(int64_t, buffer.data);
    this->serverGUID = FROM_BE(int64_t, buffer.data + 8);
    // Magic 16 bytes
    const auto n = FROM_BE(uint16_t, buffer.data + 32);
    if (buffer.length < 34 + n) {
        return std::string(std::format("Packet too short, need {}, have {} bytes", buffer.length - 34, n));
    }
    this->data = std::string(buffer.data + 34, buffer.data + 34 + n);
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::UnconnectedPongPacket::encode() const {
    const uint16_t n = this->data.length();
    auto buffer = std::make_unique<std::vector<uint8_t>>(35+n);
    buffer->at(0) = ID_UNCONNECTED_PONG;
    TO_BE(int64_t, this->pingTime, buffer->data() + 1);
    TO_BE(int64_t, this->serverGUID, buffer->data() + 9);
    std::ranges::copy(unconnectedMessageSequence, buffer->data() + 17);
    TO_BE(uint16_t, n, buffer->data() + 33);
    std::ranges::copy(this->data, buffer->data() + 35);
    return buffer;
}