#pragma once
#include <unordered_map>
#include "Chacha20Encryption.hpp"

namespace TCPIP
{
    class ClientEncryptionManager
    {
    public:
        IEncryption& getEncryption(unsigned int ID);
        void addEncryption(unsigned int id, std::unique_ptr<IEncryption> encryption);

    private:
        std::unordered_map<unsigned int, std::unique_ptr<IEncryption>> clientEncryption;
    };
}