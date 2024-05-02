#pragma once
#include <string>

namespace TCPIP {
    class FileInfo {
    public:
        FileInfo(size_t fileSize, size_t pid, std::string senderIP, std::string fileName)
                : fileSize(fileSize), pid(pid), senderIP(std::move(senderIP)), fileName(std::move(fileName)) {}

        size_t fileSize;
        size_t pid;
        std::string senderIP;
        std::string fileName;
        // attributes
    };
}