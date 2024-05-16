#pragma once
#include "BufferedFileReader.hpp"

namespace TCPIP
{
    class BufferedReader : public ::BufferedReader<TCPIPTag>
    {
    public:
        explicit BufferedReader(std::string fileName,
                                typename TCPIPTag::QueueType queue) :
                ::BufferedReader<TCPIPTag>(std::move(fileName), std::move(queue))
        { ; }

        void read() override
        {
            // Get the buffer to read into
            auto buf = queue->getFreeBuffer().value();

            // Determine amount of bytes to read and store in the buffer
            if (currentOffset + BUFFER_SIZE <= fileSize)
            {
                buf.bytesUsed = BUFFER_SIZE;
            }
            else
            {
                buf.bytesUsed = fileSize - currentOffset;
            }

            auto readCount = std::min(currentOffset + BUFFER_SIZE, fileSize);

            if (setvbuf(fileDesc, reinterpret_cast<char*>(buf.getData() + RequestHeader::StructureSizeNoAligment()), _IOFBF, BUFFER_SIZE))
            {
                throw std::runtime_error("can`t set buffering mode");
            }

            fgetc(fileDesc);

            currentOffset = readCount;

            if (currentOffset < fileSize)
            {
                fseek(fileDesc, BUFFER_SIZE - 1, SEEK_CUR);
            }
            else
            {
                readFinished.store(true);
                queue->finishRead();
            }

            queue->returnBuffer(std::move(buf));
        }
    };

}
