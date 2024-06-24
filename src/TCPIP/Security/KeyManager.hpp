#pragma once
#include <unordered_map>
#include "IKeyManager.hpp"

namespace TCPIP
{
    class KeyManager : public IKeyManager
    {
    public:
        void addKey(unsigned int id, AbstractKey key) override;
        AbstractKey& getKey(unsigned int id) override;
        void eraseKey(unsigned int id) override;
        bool keyExists(unsigned int id) override;

    private:
        std::unordered_map<unsigned int, AbstractKey> keys;
    };
}