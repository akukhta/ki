#pragma once
#include "ICopyTool.hpp"
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../queue/IQueue.hpp"
#include "StopWatch.h"
#include <memory>
#include <thread>

/// Multithreaded copy tool
/// Performs copying of a file in 2 threads:
/// One reading thread and one writing thread
/// Reading and Writing are implemented with memory mapped files
template <class ChunkType>
class ParallelCopyTool : public ICopyTool
{
public:

    /// \param fileReader pointer to the reader object
    /// \param fileWriter pointer to the writer object
    /// \param queue pointer to the queue
    ParallelCopyTool(std::unique_ptr<MMapFileReader<ChunkType>> fileReader,
        std::unique_ptr<MMapFileWriter<ChunkType>> fileWriter, std::unique_ptr<IQueue<ChunkType>> queue) :
            fileReader(std::move(fileReader)), fileWriter(std::move(fileWriter)), queue(std::move(queue))
        {
            this->queue->open();
        }

    /// Performs the copying of the file
    /// Reading performed in the calling thread
    /// Writing performed in the dedicated thread
    void copy() override
    {
        fileReader->open();
        fileWriter->create();

        writingThread = std::jthread(&ParallelCopyTool::writingFunction, this);

        // Read src file in buffered mode until we read the entire file
        while (!fileReader->isReadFinished())
        {
            queue->push(fileReader->read());
        }

        // Closing the queue when reading is done, to signalise writing thread
        // not to wait if there is no buffers to write and just return
        // since the reading is done and all data has been writen
        queue->close();

        // Manually finish the reading to close memory mapping immediately and to signalize the writing
        // thread that reading is over
        fileReader->finishRead();
    }

    /// Since All data members are RAII classes, follow the rule of zero
    /// default destructor is left intentionally, but can be removed
    ~ParallelCopyTool() override = default;

private:

    /// Writing function that should be ran from a different thread
    void writingFunction()
    {
        // Write file in buffered mode until:
        // 1. The file reading is still in progress
        // 2. The file reading is finished and the buffer queue is empty
        while (!fileReader->isReadFinished() || !queue->isEmpty())
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
        // Manually finish the reading to close memory mapping immediately
        fileWriter->finishWrite();
    }

    std::unique_ptr<MMapFileReader<ChunkType>> fileReader;
    std::unique_ptr<MMapFileWriter<ChunkType>> fileWriter;
    std::unique_ptr<IQueue<ChunkType>> queue;

    StopWatch sw = StopWatch::createAutoStartWatch("mmap copy tool benchmark");

    std::jthread writingThread;
};