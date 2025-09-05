#include "OpenConnectionRequest1Packet.h"

std::optional<std::string> RakNet::OpenConnectionRequest1Packet::Decode(Buffer buffer) {
    if (buffer.length < 17) {
        return std::string("Packet too short");
    }
    this->clientProtocol = buffer.data[16];
    this->mtu = buffer.length + 20 + 8 + 1; // Headers + packet ID.
    return std::nullopt;
}

std::expected<std::unique_ptr<std::vector<uint8_t>>, std::string> RakNet::OpenConnectionRequest1Packet::Encode() const {
    auto buffer = std::make_unique<std::vector<uint8_t>>(this->mtu-20-8); // IP Header: 20 Bytes, UDP Header: 8 Bytes
    buffer->at(0) = ID_OPEN_CONNECTION_REQUEST1;
    std::ranges::copy(unconnectedMessageSequence, buffer->data() + 17);
    buffer->at(17) = this->clientProtocol;
    return buffer;
}
