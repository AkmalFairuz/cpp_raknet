#pragma once
#include <Connection.h>
#include <include.h>

#define RAKNET_MAX_READ_BUFFER 1500

namespace RakNet {

class Listener {
    public:
        Listener(const std::string& address, uint16_t port);
        void listen();
        void close();
        [[nodiscard]] std::string getListenAddress() const;
        [[nodiscard]] uint16_t getListenPort() const;
        void setPongData(std::string pongData);
        [[nodiscard]] int64_t getId() const;
    private:
        int64_t id;
        int sockfd;
        uint16_t maxMTU;

        std::string pongData;
        std::mutex pongDataMutex;
        std::string listenAddress;
        uint16_t listenPort;
        int64_t cookieSeed;

        std::unordered_map<UdpEndpoint, std::shared_ptr<Connection>, UdpEndpointHash> connections;
        std::optional<std::string> handle(UdpEndpoint source, std::span<uint8_t> buffer);
        std::optional<std::string> handleUnconnected(UdpEndpoint source, std::span<uint8_t> buffer);
        [[nodiscard]] std::optional<std::string> handleUnconnectedPing(UdpEndpoint source, std::span<uint8_t> buffer) const;
        [[nodiscard]] std::optional<std::string> handleOpenConnectionRequest1(UdpEndpoint source, std::span<uint8_t> buffer) const;
        [[nodiscard]] std::optional<std::string> handleOpenConnectionRequest2(UdpEndpoint source, std::span<uint8_t> buffer);
        void sendUnconnected(UdpEndpoint destination, std::unique_ptr<std::vector<uint8_t>> buffer) const;
        [[nodiscard]] uint32_t getCookie(UdpEndpoint source) const;
};

}