#include "MMapFileReader.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

MMapFileReader::MMapFileReader(std::string fileName) : fileName(std::move(fileName))
{
    fileDesc = ::open(this->fileName.data(), O_RDONLY, S_IRUSR);
    
    if (fileDesc == -1)
    {
        throw std::runtime_error("can`t open src file");
    }    

    fileSize = std::filesystem::file_size(this->fileName);
}

void MMapFileReader::open()
{
    mmappedFile = static_cast<unsigned char*>(mmap(0, fileSize, PROT_READ, MAP_SHARED, fileDesc, 0));

    if (mmappedFile == MAP_FAILED)
    {
        throw std::runtime_error("Reader: Can`t create a memory mapping of input file");
    }

    if (fileSize < currentOffset)
    {
        readFinished.store(false);
    }
}

std::vector<unsigned char> MMapFileReader::read()
{
    std::vector<unsigned char> buf;
    buf.reserve(defaultBufferSize);
    
    auto readCount = std::min(currentOffset + defaultBufferSize, fileSize);

    std::copy(mmappedFile + currentOffset, mmappedFile + readCount, std::back_inserter(buf));

    currentOffset = readCount;

    if (currentOffset >= fileSize)
    {
        readFinished.store(true);
    }

    return buf;
}

bool MMapFileReader::isReadFinished() const
{
    return readFinished.load();
}

size_t MMapFileReader::getFileSize()
{
    return fileSize;
}

void MMapFileReader::finishRead()
{
    // Should i even handle if munmap fails?
    // man 2 mmap example does not provide any handling,
    // but for mmap it does
    munmap(mmappedFile, fileSize);
    close(fileDesc);
} 