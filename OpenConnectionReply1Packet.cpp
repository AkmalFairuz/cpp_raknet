#include "OpenConnectionReply1Packet.h"

std::optional<std::string> RakNet::OpenConnectionReply1Packet::Decode(Buffer buffer) {
    if (buffer.length < 27 || buffer.length < 27+static_cast<int>(buffer.data[24])*4) {
        return std::nullopt;
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