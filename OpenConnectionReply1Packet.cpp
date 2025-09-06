#include "OpenConnectionReply1Packet.h"

std::optional<std::string> RakNet::OpenConnectionReply1Packet::decode(Buffer buffer) {
    if (buffer.length < 27 || buffer.length < 27+static_cast<int>(buffer.data[24])*4) {
        return std::string("Packet too short");
    }
    auto offset = 0;
    this->serverGUID = FROM_BE(int64_t, buffer.data + 16);
    this->serverHasSecurity = buffer.data[24] != 0;
    if (this->serverHasSecurity) {
        offset = 4;
        this->cookie = FROM_BE(uint32_t, buffer.data + 25);
    }
    this->mtu = FROM_BE(uint16_t, buffer.data + 25 + offset);
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::OpenConnectionReply1Packet::encode() const {
    auto buffer = std::make_unique<std::vector<uint8_t>>(28+(
        this->serverHasSecurity ? 4 : 0));
    buffer->at(0) = ID_OPEN_CONNECTION_REPLY1;
    TO_BE(int64_t, this->serverGUID, buffer->data()+1);
    buffer->at(9) = this->serverHasSecurity ? 1 : 0;
    TO_BE(uint32_t, this->cookie, buffer->data()+10);
    TO_BE(uint16_t, this->mtu, buffer->data()+14);
    return buffer;
}