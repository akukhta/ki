#pragma once
#include <string>
#include <sys/stat.h>
#include <filesystem>
#include "../../common/Serializer.hpp"
#include "../Common/Utiles.hpp"

namespace TCPIP {

    /// Class contains the information about a file a client sends to the server
    class FileInfo {
    public:
        FileInfo() = default;

        explicit FileInfo(std::string fileName, size_t fileSize)
                : fileName(Utiles::getFileNameOnly(fileName)), fileSize(fileSize)
        {
            struct stat fileAttributes;
            stat(this->fileName.c_str(), &fileAttributes);
            filePermissions = fileAttributes.st_mode;
        }

        explicit FileInfo(std::string fileName)
            : FileInfo(fileName, std::filesystem::file_size(fileName))
        {
        }

        static FileInfo deserialize(unsigned char const* externalBuffer)
        {
            FileInfo info;
            size_t offset = 0;

            Serializer<SerializerType::NoBuffer>::deserialize(externalBuffer, info.fileSize, &offset);
            Serializer<SerializerType::NoBuffer>::deserialize(externalBuffer + offset, info.fileName, &offset);
            Serializer<SerializerType::NoBuffer>::deserialize(externalBuffer + offset, info.filePermissions);

            return info;
        }

        void serialize(std::vector<unsigned char> &buffer) const
        {
            Serializer<SerializerType::ExternalBuffer> serializer{buffer};

            serializer.serialize(fileSize);
            serializer.serialize(fileName);
            serializer.serialize(filePermissions);
        }

        size_t fileSize = 0;
        std::string fileName;
        mode_t filePermissions;
    };
}