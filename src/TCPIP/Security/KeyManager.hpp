#pragma once
#include <unordered_map>

namespace TCPIP
{
    template <typename KeyType>
    class KeyManager
    {
    public:
        void addKey(unsigned int id, KeyType key)
        {
            keys.insert({id, std::move(key)});
        }

        KeyType& getKey(unsigned int id)
        {
            if (keys.find(id) != keys.end())
            {
                return keys[id];
            }
        }

        void eraseKey(unsigned int id)
        {
            if (keys.find(id) != keys.end())
            {
                keys.erase(id);
            }
        }

        bool keyExists(unsigned int id)
        {
            return keys.find(id) == keys.end();
        }

    private:
        std::unordered_map<unsigned int, KeyType> keys;
    };
}