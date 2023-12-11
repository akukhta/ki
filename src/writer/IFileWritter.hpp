#include <vector>

class IFileWritter
{
public:
    virtual void write(std::vector<unsigned char>) = 0;
    virtual void create() = 0;
    virtual void finishWrite(bool) = 0;
    
    virtual ~IFileWritter() = 0;
};