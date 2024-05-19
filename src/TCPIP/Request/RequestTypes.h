#pragma once
#include <utility>
namespace TCPIP
{
    enum class RequestType : char {CLIENT_CONNECTED, CLIENT_DISCONNECTED, RECEIVING_REQUEST, REQUEST_RECEIVED, FILE_INFO_RECEIVED, FILE_CHUNK_RECEIVED, REQUEST_OK, REQUEST_TRY_AGAIN, REQUEST_ERROR};
}

namespace std
{
    template <>
    struct hash<TCPIP::RequestType>
    {
        size_t operator()(TCPIP::RequestType const &type) const
        {
            return hash<char>()(to_underlying(type));
        }
    };
}