#include "Listener.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <utility>

#include "OpenConnectionReply1Packet.h"
#include "OpenConnectionRequest1Packet.h"
#include "UnconnectedPingPacket.h"
#include "UnconnectedPongPacket.h"

std::atomic<int64_t> listenerID = 0;

RakNet::Listener::Listener(const std::string& address, uint16_t port) {
    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->sockfd < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
    serverAddr.sin_port = htons(port);
    if (bind(this->sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        ::close(this->sockfd);
        this->sockfd = -1;
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }
    this->connections = std::unordered_map<UdpEndpoint, std::shared_ptr<Connection>, UdpEndpointHash>(100);
    this->id = ++listenerID;
    this->listenAddress = address;
    this->listenPort = port;
    this->maxMTU = 1492;
    this->cookieSeed = time(nullptr);
}

int64_t RakNet::Listener::getId() const {
    return this->id;
}

std::string RakNet::Listener::getListenAddress() const {
    return this->listenAddress;
}

uint16_t RakNet::Listener::getListenPort() const {
    return this->listenPort;
}

void RakNet::Listener::close() {
    if (this->sockfd >= 0) {
        ::close(this->sockfd);
        this->sockfd = -1;
    }
}

void RakNet::Listener::listen() {
    uint8_t buffer[1500];
    while(true){
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        const ssize_t recvLen = recvfrom(this->sockfd, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
        if (recvLen < 0) {
            throw std::runtime_error("Failed to receive data: " + std::string(strerror(errno)));
        }
        if (recvLen == 0) {
            continue;
        }
        this->handle(toUdpEndpoint(clientAddr), Buffer{buffer, static_cast<size_t>(recvLen)});
    }
}

std::optional<std::string> RakNet::Listener::handle(UdpEndpoint source, Buffer packet) {
    if(const auto it = connections.find(source); it != connections.end()){
        // TODO:
        return std::nullopt;
    }
    return this->handleUnconnected(source, packet);
}

std::optional<std::string> RakNet::Listener::handleUnconnected(UdpEndpoint source, Buffer buffer) {
    const uint8_t pid = buffer.data[0];
    buffer = buffer.skip(1);
    switch(pid){
        case ID_UNCONNECTED_PING:
            return this->handleUnconnectedPing(source, buffer);
        case ID_OPEN_CONNECTION_REQUEST1:
            return this->handleOpenConnectionRequest1(source, buffer);
        case ID_OPEN_CONNECTION_REQUEST2:
            return this->handleOpenConnectionRequest2(source, buffer);
        default:
            if ((pid&0x80) != 0) {
                return std::nullopt;
            }
    }
    return std::nullopt;
}

std::optional<std::string> RakNet::Listener::handleUnconnectedPing(const UdpEndpoint source, const Buffer buffer) const {
    UnconnectedPingPacket packet{};
    if (auto err = packet.decode(buffer); err.has_value()) {
        return err.value();
    }

    UnconnectedPongPacket response{};
    response.pingTime = packet.pingTime;
    response.serverGUID = this->id;
    response.data = this->pongData;
    this->sendUnconnected(source, std::move(response.encode().value()));
    return std::nullopt;
}

std::optional<std::string> RakNet::Listener::handleOpenConnectionRequest1(const UdpEndpoint source, const Buffer buffer) const {
    OpenConnectionRequest1Packet packet{};
    if (auto err = packet.decode(buffer); err.has_value()) {
        return err.value();
    }

    OpenConnectionReply1Packet response{};
    response.serverHasSecurity = true;
    response.cookie = this->getCookie(source);
    response.serverGUID = this->id;
    uint16_t mtu = packet.mtu;
    if (mtu > this->maxMTU) {
        mtu = this->maxMTU;
    }
    response.mtu = mtu;
    this->sendUnconnected(source, std::move(response.encode().value()));
    return std::nullopt;
}

std::optional<std::string> RakNet::Listener::handleOpenConnectionRequest2(UdpEndpoint source, Buffer buffer) {
    return std::nullopt;
}

void RakNet::Listener::sendUnconnected(const UdpEndpoint destination, const std::unique_ptr<std::vector<uint8_t>> buffer) const {
    auto destAddr = fromUdpEndpoint(destination);
    if (sendto(this->sockfd, buffer->data(), buffer->size(), 0, reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr)) < 0) {
        throw std::runtime_error("Failed to send data: " + std::string(strerror(errno)));
    }
}

uint32_t RakNet::Listener::getCookie(const UdpEndpoint source) const {
    auto b = std::vector<uint8_t>(10);
    TO_LE(int64_t, this->cookieSeed, b.data());
    TO_LE(uint32_t, source.addr.s_addr, b.data()+8);
    TO_LE(uint32_t, source.port, b.data()+12);

    uint32_t hash = 0x811c9dc5;
    for (const auto x : b) {
        hash ^= x;
        hash *= 0x01000193;
    }
    return hash;
}

void RakNet::Listener::setPongData(std::string pongData) {
    this->pongDataMutex.lock();
    this->pongData = std::move(pongData);
    this->pongDataMutex.unlock();
}
