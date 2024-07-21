#pragma once
#include "../Request/RequestHandler.hpp"
#include "../Security/IEncryption.hpp"
#include "../Security/RSAKey.hpp"
#include "../Security/Chacha20Key.hpp"
#include "../Security/KeyManager.hpp"

namespace TCPIP
{
    class SecureRequestHandler : public RequestHandler
    {
    public:
        SecureRequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue, std::shared_ptr<MultiFileWriter> writer, std::shared_ptr<IEncryption> encryption, std::shared_ptr<IEncryption> rsaEncryption, std::shared_ptr<KeyManager<Chacha20Key>> keyManager, std::shared_ptr<RSAKey> serverRSAKey, std::shared_ptr<FileLogger> logger = nullptr);

    protected:
        void handle(std::stop_token token) override;

    private:
        static std::unordered_map<TCPIP::RequestType, std::function<void(TCPIP::SecureRequestHandler&, std::shared_ptr<TCPIP::ClientRequest>&)>> handlerFunctions;

        enum class EncryptionState {DECRYPT, DECRYPTED, ENCRYPT};
        using EncryptedRequest = std::tuple<EncryptionState, std::shared_ptr<ClientRequest>>;

        std::queue<EncryptedRequest> requests;
        std::shared_ptr<IEncryption> encryption;
        std::shared_ptr<KeyManager<Chacha20Key>> keyManager;
        std::shared_ptr<IEncryption> rsaEncryption;
        std::shared_ptr<RSAKey> serverRSAKey;

        EncryptionState& getEncryptionState(EncryptedRequest &encryptedRequest);
        std::shared_ptr<ClientRequest>& getRequest(EncryptedRequest& encryptedRequest);

        void keyExchange(std::shared_ptr<ClientRequest>& request);

        void handleEncryption(EncryptedRequest& request);
        void handleRequests(EncryptedRequest& request);
    };
}