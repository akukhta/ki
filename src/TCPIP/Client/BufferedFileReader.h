#pragma once
#include "../../reader/BufferedFileReader.hpp"

namespace TCPIP
{
    /// Class that handles reading of a file in chunks
    /// to send them to the server
    class BufferedReader : public ::BufferedReader<TCPIPTag>
    {
    public:
        explicit BufferedReader(typename TCPIPTag::QueueType queue) :
                ::BufferedReader<TCPIPTag>(std::move(queue))
        { ; }

        /// Helder function to start reading of a concrete file
        /// Since ::BufferedReader (parent) can read only one file in its lifetime
        /// This function should be used before reading the file
        /// In order to open descriptors and set needed flags
        /// \param fileName Name of file the reading is going to process
        void startReading(std::string const&fileName)
        {
            this->fileName = fileName;
            open();
        }

        /// Read a chunk of file
        /// It's override because the parent's implementation is not aware
        /// of the header that is placed at the begging of the chunk
        /// that is not the actual file's data and should not increase read offset
        void read() override
        {
            // Get the buffer to read into
            auto buf = queue->getFreeBuffer().value();

            // Determine amount of bytes to read and store in the buffer
            if (currentOffset + BUFFER_SIZE - RequestHeader::noAligmentSize() <= fileSize)
            {
                buf.bytesUsed = BUFFER_SIZE - RequestHeader::noAligmentSize();
            }
            else
            {
                buf.bytesUsed = fileSize - currentOffset;
            }

            auto readCount = std::min(currentOffset + BUFFER_SIZE - RequestHeader::noAligmentSize(), fileSize);

            if (setvbuf(fileDesc, reinterpret_cast<char*>(buf.getRequestData()), _IOFBF, BUFFER_SIZE - RequestHeader::noAligmentSize()))
            {
                throw std::runtime_error("can`t set buffering mode");
            }

            fgetc(fileDesc);

            currentOffset = readCount;

            if (currentOffset < fileSize)
            {
                fseek(fileDesc, BUFFER_SIZE - RequestHeader::noAligmentSize() - 1, SEEK_CUR);
            }
            else
            {
                readFinished.store(true);
            }

            queue->returnBuffer(std::move(buf));
        }
    };

}
