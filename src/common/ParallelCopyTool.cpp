#include "ICopyTool.hpp"
#include "../reader/IFileReader.hpp"
#include "../writer/IFileWritter.hpp"
#include "../queue/IQueue.hpp"
#include <memory>
#include <thread>

class ParallelCopyTool : public ICopyTool
{
public:
    ParallelCopyTool(std::unique_ptr<IFileReader> fileReader,
        std::unique_ptr<IFileWritter> fileWriter, std::shared_ptr<IQueue> shQueue);

    virtual void copy() override;

private:
    std::jthread writingThread;
    std::unique_ptr<IFileReader> fileReader;
    std::unique_ptr<IFileWritter> fileWriter;
    std::shared_ptr<IQueue> shQueue;
};