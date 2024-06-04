#pragma once
#include <unordered_map>
#include <memory>
#include "../../writer/IFileWriter.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "../Common/FileInfo.hpp"
#include "TCPIPQueue.hpp"
#include <fstream>
#include <thread>
#include <functional>

class ToolFactory;

namespace TCPIP {
    /// Class writes file chunks received from clients to the filesystem
    class MultiFileWriter {
    private:
        using queueType = std::shared_ptr<TCPIP::FixedBufferQueue>;

    public:
        explicit MultiFileWriter(queueType queue);

        /// Add a new file to the list of processed files
        void registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo, std::string senderIP);

        /// Write first chunk available to the corresponding file
        void write();
    private:
        /// Finish write of file
        void finishWriteOfFile(unsigned int ID);

        /// Validate client id
        bool checkClientID(unsigned int clientID);

        /// Function to apply attributes to the file
        void applyFileAttributes(unsigned int ID);

        /// Queue (memory pool)
        queueType queue;

        // Extended File Info Structure contains:
        // amount of bytes of the file written to the file system
        // the ip address of the client that is sending the file
        using ExtendedFileInfo = std::tuple<TCPIP::FileInfo, size_t, std::string>;

        std::unordered_map<unsigned int, ExtendedFileInfo> pendingFileInfo;
        std::unordered_map<unsigned int, std::FILE*> filesDescs;

        /// Set callback function that is called when write of file is finished
        void setFileWriteFinished(std::function<void(int)> cb);

        /// Callback
        std::function<void(int)> fileWriteFinished;

        std::mutex mutex;

        /// Root directory where files are going to be store
        std::string rootDir;

        friend class ::ToolFactory;

        // Helper functions
        static TCPIP::FileInfo &fileInfo(ExtendedFileInfo &fileInfoExt);
        static size_t &bytesWritten(ExtendedFileInfo &fileInfoExt);
        static std::string &senderIp(ExtendedFileInfo &fileInfoExt);
    };
}

