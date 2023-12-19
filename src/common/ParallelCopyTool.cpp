#include "ParallelCopyTool.hpp"
#include <mutex>

ParallelCopyTool::ParallelCopyTool(std::unique_ptr<MMapFileReader> fileReader,
        std::unique_ptr<MMapFileWriter> fileWriter, std::unique_ptr<IQueue> queue)
        : fileReader(std::move(fileReader)), fileWriter(std::move(fileWriter)), queue(std::move(queue))
{
    this->queue->open();
}

void ParallelCopyTool::copy()
{
    fileReader->open();
    fileWriter->create();

    writingThread = std::jthread(&ParallelCopyTool::writingFunction, this);

    while (!fileReader->isReadFinished())
    {
        queue->push(fileReader->read());
    }

    queue->close();
    fileReader->finishRead();
}

void ParallelCopyTool::writingFunction()
{
    while (true)
    {
        auto buf = queue->pop();
        
        if (buf)
        {
            fileWriter->write(std::move(*buf));
        }
        else
        {
            break;
        }
    }

    fileWriter->finishWrite();
}

ParallelCopyTool::~ParallelCopyTool()
{
    fileReader->finishRead();
    fileWriter->finishWrite();
    queue->close();
}