#pragma once

#include <cstdint>
#include <vector>

namespace RakNet {

const std::vector<uint8_t> unconnectedMessageSequence = {0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78};

enum PacketID : uint8_t {
    ID_CONNECTED_PING = 0x00,
    ID_UNCONNECTED_PING = 0x01,
    ID_UNCONNECTED_PONG = 0x1C,
    ID_OPEN_CONNECTION_REQUEST1 = 0x05,
    ID_OPEN_CONNECTION_REPLY1 = 0x06,
    ID_OPEN_CONNECTION_REQUEST2 = 0x07,
    ID_OPEN_CONNECTION_REPLY2 = 0x08,
    ID_CONNECTION_REQUEST = 0x09,
    ID_CONNECTION_REQUEST_ACCEPTED = 0x10,
    ID_NEW_INCOMING_CONNECTION = 0x13,
    ID_DISCONNECTION_NOTIFICATION = 0x15,
    ID_INCOMPATIBLE_PROTOCOL_VERSION = 0x19,
};

}

template <typename T>
void fromBytesBE(T* val, const void* ptr) {
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
    std::memcpy(val, ptr, sizeof(T));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    if constexpr (sizeof(T) == 2)
        *val = __builtin_bswap16(*val);
    else if constexpr (sizeof(T) == 4)
        *val = __builtin_bswap32(*val);
    else if constexpr (sizeof(T) == 8)
        *val = __builtin_bswap64(*val);
#endif
}

template <typename T>
void fromBytesLE(T* val, const void* ptr) {
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
    std::memcpy(&val, ptr, sizeof(T));

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if constexpr (sizeof(T) == 2)
        tmp = __builtin_bswap16(tmp);
    else if constexpr (sizeof(T) == 4)
        tmp = __builtin_bswap32(tmp);
    else if constexpr (sizeof(T) == 8)
        tmp = __builtin_bswap64(tmp);
#endif
}

template <typename T>
void toBytesBE(T val, void* ptr) {
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    if constexpr (sizeof(T) == 2)
        val = __builtin_bswap16(val);
    else if constexpr (sizeof(T) == 4)
        val = __builtin_bswap32(val);
    else if constexpr (sizeof(T) == 8)
        val = __builtin_bswap64(val);
#endif
    std::memcpy(ptr, &val, sizeof(T));
}

template <typename T>
void toBytesLE(T val, void* ptr) {
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if constexpr (sizeof(T) == 2)
        val = __builtin_bswap16(val);
    else if constexpr (sizeof(T) == 4)
        val = __builtin_bswap32(val);
    else if constexpr (sizeof(T) == 8)
        val = __builtin_bswap64(val);
#endif
    std::memcpy(ptr, &val, sizeof(T));
}