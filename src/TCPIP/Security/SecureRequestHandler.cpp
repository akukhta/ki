#include "SecureRequestHandler.hpp"

TCPIP::SecureRequestHandler::SecureRequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue,
    std::shared_ptr<MultiFileWriter> writer, std::unique_ptr<IEncryption> encryption, std::shared_ptr<IEncryption> rsaEncryption, std::shared_ptr<IKeyManager> keyManager,
    std::shared_ptr<RSAKey> serverRSAKey, std::shared_ptr<FileLogger> logger)
    : RequestHandler(std::move(queue), std::move(writer), std::move(logger)),
        encryption(std::move(encryption)), rsaEncryption(std::move(rsaEncryption)),
        keyManager(std::move(keyManager)), serverRSAKey(serverRSAKey)
{
}

void TCPIP::SecureRequestHandler::handle(std::stop_token token)
{
    while (!token.stop_requested())
    {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this](){return !requests.empty();});

        auto request = requests.front();
        requests.pop();

        handleEncryption(request);
        handleRequests(request);
    }
}

TCPIP::SecureRequestHandler::EncryptionState& TCPIP::SecureRequestHandler::getEncryptionState
    (EncryptedRequest &encryptedRequest)
{
    return std::get<0>(encryptedRequest);
}

std::shared_ptr<TCPIP::ClientRequest>& TCPIP::SecureRequestHandler::getRequest
    (TCPIP::SecureRequestHandler::EncryptedRequest &encryptedRequest)
{
    return std::get<1>(encryptedRequest);
}

void TCPIP::SecureRequestHandler::handleEncryption(TCPIP::SecureRequestHandler::EncryptedRequest &request)
{
    switch(getEncryptionState(request))
    {
        case EncryptionState::DECRYPT:
        {
            if (keyManager->keyExists(getRequest(request)->buffer->owningClientID))
            {
                TCPIP::AbstractKey &encryptionKey = keyManager->getKey(getRequest(request)->buffer->owningClientID);
                encryption->decrypt(*getRequest(request)->buffer.get(), encryptionKey);
            }
            else
            {
                rsaEncryption->decrypt(*getRequest(request)->buffer.get(), *serverRSAKey.get());
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

void TCPIP::SecureRequestHandler::handleRequests(TCPIP::SecureRequestHandler::EncryptedRequest &request)
{
    auto requestType = getRequest(request)->getRequestType();

    if (handlerFunctions.find(requestType) != handlerFunctions.end())
    {
        handlerFunctions[requestType](*this, getRequest(request));
    }
    else
    {
        if(logger)
        {
            logger->log("Incorrect handler");
        }
    }
}
