#include <memory>
#include <filesystem>
#include <vector>
#include <chrono>
#include <fstream>
#include <iterator>

class TmpFile
{
public:
    TmpFile(std::string const &fileName, std::vector<unsigned char> content)
         : fileName(fileName), content(std::move(content)), fileSize(this->content.size()) 
    {}

    TmpFile(std::string const &fileName) : fileName(fileName), fileSize(0)
    {}

    TmpFile(TmpFile const &) = delete;
    TmpFile& operator=(TmpFile const &) = delete;

    ~TmpFile()
    {
        std::filesystem::remove(fileName);
    }
    
    static TmpFile createEmpty()
    {
        auto fName = std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::ofstream out(fName, std::ios_base::binary);
        out.close();
        return TmpFile(fName);
    }

    static TmpFile create(size_t fileSize = 500000)
    {
        auto fName = std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());

        std::vector<unsigned char> content;

        for (size_t i = 0; i < fileSize; i++)
        {
            unsigned char val = rand();
            
            content.push_back(val);
        }

        std::ofstream out(fName, std::ios_base::binary);
        std::copy(content.begin(), content.end(), std::ostream_iterator<unsigned char>(out));

        out.close();

        return TmpFile(fName, std::move(content));
    }

    std::string fileName;
    std::vector<unsigned char> content;
    size_t fileSize;
};