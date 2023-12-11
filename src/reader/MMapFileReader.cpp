#include "MMapFileReader.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdexcept>

MMapFileReader::MMapFileReader(std::string_view const & fileName)
{
    if (fileDesc = ::open(fileName.data(), O_RDONLY) < 0)
    {
        throw std::runtime_error("can`t open src file");
    }

    struct stat statvar;

    if (fstat(fileDesc, &statvar) < 0)
    {
        throw std::runtime_error("can`t query size of src file");
    }

    fileSize = statvar.st_size;

    mmappedFile = static_cast<unsigned char*>(mmap(0, fileSize, PROT_READ, MAP_SHARED, fileDesc, 0));

    if (!mmappedFile)
    {
        throw std::runtime_error("can`t create a memory mapping of input file");
    }
}

std::vector<unsigned char> MMapFileReader::read()
{
    return std::vector<unsigned char>();
}