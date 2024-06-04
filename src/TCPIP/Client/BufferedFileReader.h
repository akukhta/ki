#pragma once
#include "../../reader/BufferedFileReader.hpp"

namespace TCPIP
{
    class BufferedReader : public ::BufferedReader<TCPIPTag>
    {
    public:
        explicit BufferedReader(typename TCPIPTag::QueueType queue) :
                ::BufferedReader<TCPIPTag>(std::move(queue))
        { ; }

        void startReading(std::string const&fileName)
        {
            this->fileName = fileName;
            open();
        }

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
