#pragma once
#include <memory>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <utility>
#include <stdexcept>
#include <iostream>
#include "ICopyTool.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "StopWatch.h"

class IPCTool : public ICopyTool
{
public:
    IPCTool(std::unique_ptr<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileReader,
            std::unique_ptr<BufferedFileWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileWriter,
            FixedBufferQueue<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>* queue,
            std::shared_ptr<SharedMemoryManager> SharedMemManager)
            : fileReader(std::move(fileReader)), fileWriter(std::move(fileWriter)), queue(queue), SharedMemManager(std::move(SharedMemManager)),
                sw(StopWatch::createAutoStartWatch("ipc copy tool benchmark"))
            {}

    virtual void copy() override
    {
        if (SharedMemManager->isFirstProcess())
        {
            if (!fileReader)
            {
                throw std::runtime_error("FileReader is nullptr");
            }

            std::cout << "IPC: Reading the file...\n";

            queue->open();
            fileReader->open();

            while(!fileReader->isReadFinished())
            {
                fileReader->read();
            }

            queue->close();
        }
        else
        {
            if (!fileWriter)
            {
                throw std::runtime_error("FileWriter is nullptr");
            }

            std::cout << "IPC: Writing the file...\n";

            fileWriter->create();

            while(true)
            {
                if (queue->isReadFinished() && queue->isEmpty())
                {
                    break;
                }

                fileWriter->write();
            }
        }
    }

private:

    StopWatch sw;
    std::unique_ptr<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileReader;
    std::unique_ptr<BufferedFileWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileWriter;
    FixedBufferQueue<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>* queue;
    std::shared_ptr<SharedMemoryManager> SharedMemManager;
};