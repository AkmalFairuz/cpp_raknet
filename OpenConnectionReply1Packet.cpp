#include "OpenConnectionReply1Packet.h"

std::optional<std::string> RakNet::OpenConnectionReply1Packet::decode(std::span<uint8_t> buffer) {
    if (buffer.size() < 27 || buffer.size() < 27+static_cast<int>(buffer[24])*4) {
        return std::string("Packet too short");
    }
    auto offset = 0;
    fromBytesBE(&this->serverGUID, buffer.data() + 16);
    this->serverHasSecurity = buffer[24] != 0;
    if (this->serverHasSecurity) {
        offset = 4;
        fromBytesBE(&this->cookie, buffer.data() + offset);
    }
    fromBytesBE(&this->mtu, buffer.data() + 25 + offset);
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::OpenConnectionReply1Packet::encode() const {
    auto buffer = std::make_unique<std::vector<uint8_t>>(28+(
        this->serverHasSecurity ? 4 : 0));
    buffer->at(0) = ID_OPEN_CONNECTION_REPLY1;
    std::ranges::copy(unconnectedMessageSequence, buffer->data() + 1);
    toBytesBE<int64_t>(this->serverGUID, buffer->data() + 17);
    buffer->at(25) = this->serverHasSecurity ? 1 : 0;
    toBytesBE<uint32_t>(this->cookie, buffer->data() + 10);
    toBytesBE<uint16_t>(this->mtu+4, buffer->data()+16);
    return buffer;
}