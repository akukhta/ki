#include "MMapFileWriter.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <stdexcept>
#include <sys/mman.h>

MMapFileWriter::MMapFileWriter(std::string_view fileName, size_t fileSize) 
    : fileName(fileName), fileSize(fileSize), currentOffset(0), writeFinished(false)
{
    ;
}

void MMapFileWriter::write(std::vector<unsigned char> buf)
{
    if (currentOffset + buf.size() > fileSize)
    {
        throw std::runtime_error("Writer: writing more than expected");
    }
    
    std::copy(buf.begin(), buf.end(), mmappedFile + currentOffset);

    currentOffset += buf.size();
}

void MMapFileWriter::create()
{
    fileDesc = open(fileName.data(), O_RDWR | O_CREAT, 0644);

    if (fileDesc == -1)
    {
        throw std::runtime_error("can`t create a file");
    }

    if (ftruncate(fileDesc, fileSize) == -1)
    {
        throw std::runtime_error("can`t resize a file");
    }

    mmappedFile = static_cast<unsigned char*>(mmap(0, fileSize,
         PROT_READ | PROT_WRITE, MAP_SHARED, fileDesc, 0));
    
    if (mmappedFile == MAP_FAILED)
    {
        throw std::runtime_error("Writer: Can`t create a memory mapping of input file");
    }
}

void MMapFileWriter::finishWrite()
{
    writeFinished.store(true);
    fsync(fileDesc);
    close(fileDesc);
}

bool MMapFileWriter::isWriteFinished()
{
    return writeFinished.load();
}