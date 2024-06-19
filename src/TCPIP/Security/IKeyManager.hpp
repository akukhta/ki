#pragma once
#include "AbstractKey.hpp"

namespace TCPIP
{
    class IKeyManager
    {
    public:
        virtual ~IKeyManager() = default;
        virtual void addKey(unsigned int id, AbstractKey key) = 0;
        virtual AbstractKey& getKey(unsigned int id) = 0;
        virtual void eraseKey(unsigned int id) = 0;
    };
}