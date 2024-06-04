#pragma once
#include <utility>

namespace TCPIP
{
    /// Types of client's requests send to the server
    enum class RequestType : char {FILE_INFO_RECEIVED, FILE_CHUNK_RECEIVED};

    /// Types of server's responses send to a client
    enum class ServerResponse : char {REQUEST_RECEIVED, FILE_RECEIVED, CRITICAL_ERROR};
}