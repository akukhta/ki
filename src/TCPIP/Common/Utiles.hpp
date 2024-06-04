#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>

namespace TCPIP
{
    class Utiles
    {
    public:
        static void setSocketNonBlock(int socketFD)
        {
            int flags;

            if (flags = fcntl(socketFD, F_GETFL, 0); flags == -1)
            {
                flags = 0;
            }

            fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);
        }

        static std::string getFileNameOnly(std::string const& path)
        {
            return std::filesystem::path(path).filename();
        }
    };
}