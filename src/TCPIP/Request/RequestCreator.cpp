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