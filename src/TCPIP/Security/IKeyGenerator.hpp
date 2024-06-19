#pragma once
#include "RSAKey.hpp"
#include <memory>

namespace TCPIP
{
    class IKeyGenerator
    {
    public:
        virtual ~IKeyGenerator() = default;
        virtual std::unique_ptr<RSAKey> generateKey() = 0;
    };
}