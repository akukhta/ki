#pragma once
#include <utility>

namespace TCPIP
{
    enum class RequestType : char {FILE_INFO_RECEIVED, FILE_CHUNK_RECEIVED};
    enum class ServerResponse : char {REQUEST_RECEIVED, FILE_RECEIVED, CRITICAL_ERROR};
}