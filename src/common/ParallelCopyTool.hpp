#pragma once
#include "ICopyTool.hpp"
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../queue/IQueue.hpp"
#include <memory>
#include <thread>

class ParallelCopyTool : public ICopyTool
{
public:
    ParallelCopyTool(std::unique_ptr<MMapFileReader> fileReader,
        std::unique_ptr<MMapFileWriter> fileWriter, std::unique_ptr<IQueue> queue);

    virtual void copy() override;

    ~ParallelCopyTool();
private:
    void writingFunction();

    std::unique_ptr<MMapFileReader> fileReader;
    std::unique_ptr<MMapFileWriter> fileWriter;
    std::unique_ptr<IQueue> queue;

    std::jthread writingThread;
};