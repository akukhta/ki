#include "ICopyTool.hpp"
#include "../reader/IFileReader.hpp"
#include "../writer/IFileWriter.hpp"
#include "../queue/IQueue.hpp"
#include <memory>
#include <thread>

class ParallelCopyTool : public ICopyTool
{
public:
    ParallelCopyTool(std::unique_ptr<IFileReader> fileReader,
        std::unique_ptr<IFileWriter> fileWriter, std::shared_ptr<IQueue> shQueue);

    virtual void copy() override;

private:
    std::jthread writingThread;
    std::unique_ptr<IFileReader> fileReader;
    std::unique_ptr<IFileWriter> fileWriter;
    std::shared_ptr<IQueue> shQueue;
};