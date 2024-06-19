#include <stdexcept>
#include "RSAKeyManager.hpp"

void TCPIP::RSAKeyManager::addKey(unsigned int id, TCPIP::AbstractKey key)
{
    keys.emplace(id, std::move(key));
}

TCPIP::AbstractKey &TCPIP::RSAKeyManager::getKey(unsigned int id)
{
    if (auto it = keys.find(id); it != keys.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("No RSA Key for the given ID");
    }
}

void TCPIP::RSAKeyManager::eraseKey(unsigned int id)
{
    if (keys.find(id) != keys.end())
    {
        keys.erase(id);
    }
}
