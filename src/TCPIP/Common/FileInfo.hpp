#pragma once
#include <string>
#include "../../common/Serializer.hpp"

namespace TCPIP {
    class FileInfo {
    public:
        FileInfo() = default;

        explicit FileInfo(size_t fileSize, std::string fileName)
                : fileSize(fileSize), fileName(std::move(fileName)) {}

        static FileInfo deserialize(std::vector<unsigned char> &buffer)
        {
            Serializer<SerializerType::ExternalBuffer> serializer(buffer);
            FileInfo info;

            serializer.deserialize(info.fileSize);
            serializer.deserialize(info.fileName);

            return info;
        }

        static FileInfo deserialize(unsigned char const* externalBuffer)
        {
            FileInfo info;

            Serializer<SerializerType::NoBuffer>::deserialize(externalBuffer, info.fileSize);
            Serializer<SerializerType::NoBuffer>::deserialize(externalBuffer + sizeof(size_t), info.fileName);

            return info;
        }

        std::vector<unsigned char> serialize() const
        {
            std::vector<unsigned char> buffer;
            buffer.reserve(259);

            Serializer<SerializerType::ExternalBuffer> serializer{buffer};

            serializer.serialize(fileSize);
            serializer.serialize(fileName);

            buffer.shrink_to_fit();

            return buffer;
        }

        size_t fileSize = 0;
        size_t port = 0;
        std::string senderIP;
        std::string fileName;

        size_t bytesWritten = 0;
        // attributes
    };
}