#include "RequestCreator.hpp"
#include "../../common/Serializer.hpp"
#include "../Common/FileInfo.hpp"

std::vector<unsigned char> TCPIP::RequestCreator::createFileInfoRequest(const std::string &fileName)
{
    Serializer<SerializerType::InternalBuffer> serializer;
    FileInfo info{fileName};

    serializer.serialize(std::to_underlying(TCPIP::RequestType::FILE_INFO_RECEIVED));
    serializer.serialize(short{0});

    info.serialize(serializer.getBuffer());

    serializer.overwrite(sizeof(RequestType), static_cast<short>(serializer.getBuffer().size() - sizeof(RequestType) - sizeof(short)));
    return serializer.getBuffer();
}

TCPIP::Buffer &TCPIP::RequestCreator::createFileChunkRequest(TCPIP::Buffer &buffer)
{
    auto requestData = buffer.getData();
    Serializer<SerializerType::NoBuffer>::overwrite(requestData, 0, std::to_underlying(TCPIP::RequestType::FILE_CHUNK_RECEIVED));
    Serializer<SerializerType::NoBuffer>::overwrite(requestData, sizeof(RequestHeader::type), static_cast<short>(buffer.bytesUsed));
    return buffer;
}

std::vector<unsigned char> TCPIP::RequestCreator::createKeyPairRequest(std::span<char> key, std::span<char> iv)
{
    Serializer<SerializerType::InternalBuffer> serializer;

    serializer.serialize(std::to_underlying(TCPIP::RequestType::KEY_EXCHANGE));
    serializer.serialize(short{0});
    serializer.serialize(key);
    serializer.serialize(iv);

    serializer.overwrite(sizeof(RequestType), 2 * sizeof(size_t) + key.size() + iv.size());
    return serializer.getBuffer();
}
