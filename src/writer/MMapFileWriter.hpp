#include "IFileWriter.hpp"
#include <string_view>
#include <atomic>

class MMapFileWriter : public IFileWriter
{
public:
    MMapFileWriter(std::string_view fileName, size_t fileSize);

    virtual void write(std::vector<unsigned char>) override;
    virtual void create() override;
    virtual void finishWrite(bool) override;
    
    virtual ~MMapFileWriter() {};

private:
    int fileDesc;
    size_t fileSize;

    unsigned char *mmappedFile;

    size_t currentOffset = 0;
    
    std::atomic_bool writeFinished;

    std::string_view fileName;
};