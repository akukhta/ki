#include <stdexcept>
#include "KeyManager.hpp"

void TCPIP::KeyManager::addKey(unsigned int id, TCPIP::AbstractKey key)
{
    keys.emplace(id, std::move(key));
}

TCPIP::AbstractKey &TCPIP::KeyManager::getKey(unsigned int id)
{
    if (auto it = keys.find(id); it != keys.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("No Key for the given ID");
    }
}

void TCPIP::KeyManager::eraseKey(unsigned int id)
{
    if (keys.find(id) != keys.end())
    {
        keys.erase(id);
    }
}

bool TCPIP::KeyManager::keyExists(unsigned int id)
{
    return keys.find(id) != keys.end();
}
