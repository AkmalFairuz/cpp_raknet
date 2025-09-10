#include "Listener.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif // _WIN32
#include <cstring>
#include <atomic>
#include <thread>
#include <mutex>
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
#ifdef _WIN32
        closesocket(this->sockfd);
#else
        ::close(this->sockfd);
#endif // _WIN32
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
#ifdef _WIN32
        closesocket(this->sockfd);
#else
        ::close(this->sockfd);
#endif // _WIN32
        this->sockfd = -1;
    }
}

void RakNet::Listener::listen() {
#ifdef _WIN32
    char buffer[RAKNET_MAX_READ_BUFFER];
#else
    uint8_t buffer[RAKNET_MAX_READ_BUFFER];
#endif // _WIN32
    while(true){
        sockaddr_in clientAddr{};
#ifdef _WIN32
        int clientAddrLen = sizeof(clientAddr);
#else
        socklen_t clientAddrLen = sizeof(clientAddr);
#endif // _WIN32
        const ssize_t recvLen = recvfrom(this->sockfd, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
        if (recvLen < 0) {
            throw std::runtime_error("Failed to receive data: " + std::string(strerror(errno)));
        }
        if (recvLen == 0) {
            continue;
        }
#ifdef _WIN32
        const std::span packet(reinterpret_cast<uint8_t*>(buffer), recvLen);
#else
        const std::span<uint8_t> packet(buffer, recvLen);
#endif // _WIN32
        this->handle(toUdpEndpoint(clientAddr), packet);
    }
}

std::optional<std::string> RakNet::Listener::handle(const UdpEndpoint source, std::span<uint8_t> packet) {
    if(const auto it = connections.find(source); it != connections.end()){
        // TODO:
        return std::nullopt;
    }
    return this->handleUnconnected(source, packet);
}

std::optional<std::string> RakNet::Listener::handleUnconnected(const UdpEndpoint source, std::span<uint8_t> buffer) {
    const uint8_t pid = buffer[0];
    buffer = buffer.subspan(1);
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

std::optional<std::string> RakNet::Listener::handleUnconnectedPing(const UdpEndpoint source, const std::span<uint8_t> buffer) const {
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

std::optional<std::string> RakNet::Listener::handleOpenConnectionRequest1(const UdpEndpoint source, const std::span<uint8_t> buffer) const {
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

std::optional<std::string> RakNet::Listener::handleOpenConnectionRequest2(UdpEndpoint source, std::span<uint8_t> buffer) {
    return std::nullopt;
}

void RakNet::Listener::sendUnconnected(const UdpEndpoint destination, const std::unique_ptr<std::vector<uint8_t>> buffer) const {
    auto destAddr = fromUdpEndpoint(destination);
    if (sendto(
        this->sockfd,
#ifdef _WIN32
        reinterpret_cast<const char*>(buffer->data()),
        static_cast<int>(buffer->size()),
#else
        buffer->data(),
        buffer->size(),
#endif // _WIN32
        0,
        reinterpret_cast<sockaddr*>(&destAddr),
        sizeof(destAddr)
    ) < 0) {
        throw std::runtime_error("Failed to send data: " + std::string(strerror(errno)));
    }
}

uint32_t RakNet::Listener::getCookie(const UdpEndpoint source) const {
    auto b = std::vector<uint8_t>(10);
    toBytesLE(this->cookieSeed, b.data());
    toBytesLE(source.addr.s_addr, b.data()+8);
    toBytesLE(source.port, b.data()+12);

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
