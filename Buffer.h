#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace RakNet {

class Buffer {
    public:
        Buffer(uint8_t* data, const size_t length) : data(data), length(length) {}
        uint8_t* data;
        size_t length;
        [[nodiscard]] Buffer skip(size_t bytes) const;
};

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

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define FROM_BE(type, ptr) ({ \
        type _tmp; \
        std::memcpy(&_tmp, (ptr), sizeof(type)); \
        if constexpr (sizeof(type) == 2) _tmp = __builtin_bswap16(_tmp); \
        else if constexpr (sizeof(type) == 4) _tmp = __builtin_bswap32(_tmp); \
        else if constexpr (sizeof(type) == 8) _tmp = __builtin_bswap64(_tmp); \
        _tmp; \
    })
    #define FROM_LE(type, ptr) ({ \
        type _tmp; \
        std::memcpy(&_tmp, (ptr), sizeof(type)); \
        _tmp; \
    })

    #define TO_BE(type, val, ptr) ({ \
        type _tmp = val; \
        if constexpr (sizeof(type) == 2) _tmp = __builtin_bswap16(_tmp); \
        else if constexpr (sizeof(type) == 4) _tmp = __builtin_bswap32(_tmp); \
        else if constexpr (sizeof(type) == 8) _tmp = __builtin_bswap64(_tmp); \
        std::memcpy((ptr), &_tmp, sizeof(type)); \
    })
    #define TO_LE(type, val, ptr) ({ \
        type _tmp = val; \
        std::memcpy((ptr), &_tmp, sizeof(type)); \
    })
#else
    #define FROM_BE(type, ptr) ({ \
        type _tmp; \
        std::memcpy(&_tmp, (ptr), sizeof(type)); \
        _tmp; \
    })
    #define FROM_LE(type, ptr) ({ \
        type _tmp; \
        std::memcpy(&_tmp, (ptr), sizeof(type)); \
        if constexpr (sizeof(type) == 2) _tmp = __builtin_bswap16(_tmp); \
        else if constexpr (sizeof(type) == 4) _tmp = __builtin_bswap32(_tmp); \
        else if constexpr (sizeof(type) == 8) _tmp = __builtin_bswap64(_tmp); \
        _tmp; \
    })

    #define TO_BE(type, val, ptr) ({ \
        type _tmp = val; \
        std::memcpy((ptr), &_tmp, sizeof(type)); \
    })
    #define TO_LE(type, val, ptr) ({ \
        type _tmp = val; \
        if constexpr (sizeof(type) == 2) _tmp = __builtin_bswap16(_tmp); \
        else if constexpr (sizeof(type) == 4) _tmp = __builtin_bswap32(_tmp); \
        else if constexpr (sizeof(type) == 8) _tmp = __builtin_bswap64(_tmp); \
        std::memcpy((ptr), &_tmp, sizeof(type)); \
    })
#endif