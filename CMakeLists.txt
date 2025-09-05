cmake_minimum_required(VERSION 4.0)
project(cpp_raknet)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_executable(cpp_raknet
        Connection.h
        Listener.cpp
        Listener.h
        Buffer.cpp
        Buffer.h
        UdpEndpoint.cpp
        UdpEndpoint.h
        UnconnectedPingPacket.cpp
        UnconnectedPingPacket.h
        main.cpp
        UnconnectedPongPacket.h
        UnconnectedPongPacket.cpp
        OpenConnectionRequest1Packet.cpp
        OpenConnectionRequest1Packet.h
        include.h
        OpenConnectionReply1Packet.h
        OpenConnectionReply1Packet.cpp)
