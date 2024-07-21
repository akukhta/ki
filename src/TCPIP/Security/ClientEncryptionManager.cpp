#include "ClientEncryptionManager.hpp"

TCPIP::IEncryption &TCPIP::ClientEncryptionManager::getEncryption(unsigned int ID)
{
    if (clientEncryption.find(ID) != clientEncryption.end())
    {
        return *(clientEncryption[ID].get());
    }
    else
    {
        throw std::runtime_error("No Encryption for given client");
    }
}

void TCPIP::ClientEncryptionManager::addEncryption(unsigned int id, std::unique_ptr<IEncryption> encryption)
{
    clientEncryption.insert({id, std::move(encryption)});
}
