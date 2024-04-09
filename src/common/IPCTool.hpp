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

    enum class ProcessType : char {ReaderProcess, WriterProcess, Invalid};

    IPCTool(std::unique_ptr<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileReader,
            std::unique_ptr<BufferedFileWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileWriter,
            FixedBufferQueue<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>* queue,
            std::shared_ptr<SharedMemoryManager> SharedMemManager, ProcessType toolType)
            : fileReader(std::move(fileReader)), fileWriter(std::move(fileWriter)), queue(queue), SharedMemManager(std::move(SharedMemManager)),
                sw(StopWatch::createAutoStartWatch("ipc copy tool benchmark")), ipcToolType(toolType)
            {
                size_t ProcInfo::* counterToIncrease = nullptr;

                if (ipcToolType == ProcessType::WriterProcess)
                {
                    counterToIncrease = &ProcInfo::writerProcessCount;
                }
                else if (ipcToolType == ProcessType::ReaderProcess)
                {
                    counterToIncrease = &ProcInfo::readerProcessCount;
                }

                procInfo = this->SharedMemManager->getProcInfo();

                {
                    auto lock = procInfo->createScopedLock();

                    if (!counterToIncrease || (procInfo->*counterToIncrease)) {
                        ipcToolType = ProcessType::Invalid;
                        std::cout << "The Tool with specified mode has already launched\n";
                        return;
                    }

                    (procInfo->*counterToIncrease)++;
                }
            }

            ~IPCTool()
            {
                if (procInfo)
                {
                    auto lk = procInfo->createScopedLock();

                    if (ipcToolType == ProcessType::ReaderProcess) {
                        procInfo->readerProcessCount--;
                    }
                    else if (ipcToolType == ProcessType::WriterProcess){
                        procInfo->writerProcessCount--;
                    }
                }
            }

    void copy() override
    {
        if (ipcToolType == ProcessType::ReaderProcess)
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
        else if (ipcToolType == ProcessType::WriterProcess)
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
        else
        {
            return;
        }
    }

private:
    StopWatch sw;
    std::unique_ptr<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileReader;
    std::unique_ptr<BufferedFileWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> fileWriter;
    FixedBufferQueue<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>* queue;
    std::shared_ptr<SharedMemoryManager> SharedMemManager;
    ProcInfo *procInfo = nullptr;
    ProcessType ipcToolType;
};