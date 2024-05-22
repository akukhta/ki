#include "RequestCreator.hpp"
#include <filesystem>
#include "../../common/Serializer.hpp"

std::vector<unsigned char> TCPIP::RequestCreator::createFileInfoRequest(const std::string &fileName)
{
    Serializer<SerializerType::InternalBuffer> serializer;
    serializer.serialize(std::to_underlying(TCPIP::RequestType::FILE_INFO_RECEIVED));
    serializer.serialize(short{0});
    serializer.serialize(std::filesystem::file_size(fileName));
    serializer.serialize(std::filesystem::path(fileName).filename().string());
    serializer.overwrite(sizeof(RequestType), static_cast<short>(serializer.getBuffer().size() - sizeof(RequestType) - sizeof(short)));
    return serializer.getBuffer();
}

TCPIP::Buffer &TCPIP::RequestCreator::createFileChunkRequest(TCPIP::Buffer &buffer)
{
    auto ptr = buffer.getData();
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, 0, std::to_underlying(TCPIP::RequestType::FILE_CHUNK_RECEIVED));
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, sizeof(RequestHeader::type), static_cast<short>(buffer.bytesUsed));
    return buffer;
}