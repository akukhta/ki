#include "IFileReader.hpp"
#include <string_view>

class MMapFileReader : public IFileReader
{
public:
    MMapFileReader(std::string_view const & fileName);

    virtual void open() override;
    virtual std::vector<unsigned char> read() override;
    virtual bool isReadFinished() override;

private:
    int fileDesc;
    size_t fileSize;
    unsigned char *mmappedFile;
};