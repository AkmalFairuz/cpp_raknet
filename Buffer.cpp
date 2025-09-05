#include <Buffer.h>

RakNet::Buffer RakNet::Buffer::skip(size_t bytes) const {
    if (bytes >= this->length) {
        return Buffer{nullptr, 0};
    }
    return Buffer{data + bytes, this->length - bytes};
}