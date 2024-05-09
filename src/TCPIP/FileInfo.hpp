#pragma once
#include <string>
#include "../common/Serializer.hpp"

namespace TCPIP {
    class FileInfo {
    public:
        FileInfo() = default;

        explicit FileInfo(size_t fileSize, std::string fileName)
                : fileSize(fileSize), fileName(std::move(fileName)) {}

        static FileInfo deserialize(std::vector<unsigned char> &buffer)
        {
            Serializer serializer(buffer);
            FileInfo info;

            serializer.deserialize(info.fileSize);
            serializer.deserialize(info.fileName);
            return info;
        }

        std::vector<unsigned char> serialize()
        {
            std::vector<unsigned char> buffer;
            buffer.reserve(259);

            Serializer serializer{buffer};

            serializer.serialize(fileSize);
            serializer.serialize(fileName);

            return buffer;
        }

        size_t fileSize;
        size_t port;
        std::string senderIP;
        std::string fileName;

        size_t bytesWritten = 0;
        // attributes
    };
}