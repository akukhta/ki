#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>
#include <openssl/err.h>
#define MAX_OPENSSL_ERR_LEN 256

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

        static size_t getFileSize(std::string const& fileName)
        {
            return std::filesystem::file_size(fileName);
        }

        template<typename T>
        static T satCeilingSub(T x, T y)
        {
            auto res = (x / y);
            return res != 0 ? ((x / y) + (x % y != 0 ? 1 : 0)) : 1;
        }

        static std::string getOpenSSLError()
        {
            auto errorCode = ERR_get_error();

            if (errorCode == 0)
            {
                throw std::runtime_error("No error");
            }

            std::string errorDescription(MAX_OPENSSL_ERR_LEN, '\0');
            ERR_error_string_n(errorCode, errorDescription.data(), MAX_OPENSSL_ERR_LEN);
            errorDescription.resize(errorDescription.length());

            return errorDescription;
        }
    };
}