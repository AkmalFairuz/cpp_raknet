//
// Created by Akmal Fairuz on 05/09/25.
//

#include <format>

#include "Listener.h"
#include "memory"

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed: " + std::string(strerror(iResult)));
    }

    auto listener = std::make_unique<RakNet::Listener>("0.0.0.0", 19130);
    listener->setPongData(std::format("MCPE;RakNet-CPP;827;1.21.101;123;124;{};RakNet-CPP;Creative;1;{};{};0;", listener->getId(), listener->getListenPort(), listener->getListenPort()));
    listener->listen();
}