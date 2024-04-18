#pragma once
#include "ICopyTool.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "StopWatch.h"
#include <memory>
#include <thread>

/// Buffered Version of multithread copy tool
/// The main difference from ParallelCopyTool is
/// Optimized File I/O and Memory Allocations (allocated once on stack) in BPCopyTool
class BPCopyTool : public ICopyTool
{
public:

    /// \param fileReader pointer to the reader object, the object type is strongly defined as BufferedReader<std::mutex, std::condition_variable, std::unique_lock, std::deque>
    /// to emphasize the reader class with such template arguments is used for non ipc application
    /// \param fileWriter pointer to the writer object, the object type is strongly defined as BufferedWriter<std::mutex, std::condition_variable, std::unique_lock, std::deque>
    /// to emphasize the writer class with such template arguments is used for non ipc application
    /// \param queue pointer to non-ipc queue
    BPCopyTool(std::unique_ptr<BufferedReader<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileReader,
        std::unique_ptr<BufferedFileWriter<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileWriter, std::shared_ptr<FixedBufferQueue<std::mutex, std::condition_variable, std::unique_lock, std::deque>> queue) :
            fileReader(std::move(fileReader)),
            fileWriter(std::move(fileWriter)),
            queue(std::move(queue)),
            sw(StopWatch::createAutoStartWatch("vbuf copy tool benchmark"))
        {}

    /// Performs the copying of the file
    /// Reading performed in the calling thread
    /// Writing performed in the dedicated thread
    void copy() override
    {
        queue->open();

        fileReader->open();
        fileWriter->create();

        writingThread = std::jthread(&BPCopyTool::writingFunction, this);

        // Read src file in buffered mode until we read the entire file
        while (!fileReader->isReadFinished())
        {
            fileReader->read();
        }

        // Closing the queue when reading is done, to signalise writing thread
        // not to wait if there is no buffers to write and just return
        // since the reading is done and all data has been writen
        queue->close();
    }

    /// Since All data members are RAII classes, follow the rule of zero
    /// default destructor is left intentionally, but can be removed
    ~BPCopyTool() override = default;
    
private:

    /// Writing function that should be ran from a different thread
    void writingFunction()
    {
        // Write file in buffered mode until:
        // 1. The file reading is still in progress
        // 2. The file reading is finished and the buffer queue is empty
        while (!fileReader->isReadFinished() || !queue->isEmpty())
        {
            fileWriter->write();
        }
    }

    StopWatch sw; /// StopWatch object to measure the time of copying, works in auto mode (= start the stopwatch at its construction, stop and print time taken at its destruction)

    std::unique_ptr<BufferedReader<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileReader;
    std::unique_ptr<BufferedFileWriter<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileWriter;
    std::shared_ptr<FixedBufferQueue<std::mutex, std::condition_variable, std::unique_lock, std::deque>> queue;
    
    std::jthread writingThread; /// Thread dedicated to writing operation, reading operation performed in "main" thread (the thread from which copy function has called)
};