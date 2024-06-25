#pragma once
#include "../Request/RequestHandler.hpp"
#include "../Security/IEncryption.hpp"
#include "../Security/IKeyManager.hpp"
#include "../Security/RSAKey.hpp"

namespace TCPIP
{
    class SecureRequestHandler : public RequestHandler
    {
    public:
        SecureRequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue, std::shared_ptr<MultiFileWriter> writer, std::unique_ptr<IEncryption> encryption, std::shared_ptr<IEncryption> rsaEncryption, std::shared_ptr<IKeyManager> keyManager, std::shared_ptr<RSAKey> serverRSAKey, std::shared_ptr<FileLogger> logger = nullptr);

    protected:
        void handle(std::stop_token token) override;

    private:
        enum class EncryptionState {DECRYPT, DECRYPTED, ENCRYPT};
        using EncryptedRequest = std::tuple<EncryptionState, std::shared_ptr<ClientRequest>>;

        std::queue<EncryptedRequest> requests;
        std::unique_ptr<IEncryption> encryption;
        std::shared_ptr<IKeyManager> keyManager;
        std::shared_ptr<IEncryption> rsaEncryption;
        std::shared_ptr<RSAKey> serverRSAKey;

        EncryptionState& getEncryptionState(EncryptedRequest &encryptedRequest);
        std::shared_ptr<ClientRequest>& getRequest(EncryptedRequest& encryptedRequest);

        void handleEncryption(EncryptedRequest& request);
        void handleRequests(EncryptedRequest& request);
    };
}