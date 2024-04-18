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
#include "../IPC/SiblingProcessObserver.hpp"
#include "StopWatch.h"

/// IPC version of copy tool
/// As BPCopyTool the IPCTool utilizes optimized File I/O and memory allocation (allocated once in shared memory)
/// Performs Reading and Writing in different processes
/// Utilizing shared memory
class IPCTool : public ICopyTool
{
public:
    /// \param fileReader pointer to the reader object, the object type is strongly defined as
    /// BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>
    /// to emphasize the reader class with such template arguments is used for ipc application
    /// \param fileWriter pointer to the writer object, the object type is strongly defined as
    /// BufferedWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>
    /// to emphasize the writer class with such template arguments is used for non ipc application
    /// \param queue pointer to IPC queue
    /// \param SharedMemManager pointer to a shared memory manager, object that is responsible for shared memory allocation/deallocation
    /// also provides allocators that alloate memory within shared memory region
    /// \param toolType type of currently running process (writer or reader), should be determined before the IPCTool object construction in order to
    /// correctly increment counters of Reader and Writer process
    /// that are used to correctly deallocate shared memory resources and kill idling processes after some timeout
    /// To determine what type of tool to create use bool SharedMemoryManager::isFirstProcess()
    /// First process is always reader, the second is writer
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

                timer = std::make_unique<SiblingProcessObserver>(procInfo,
                    ipcToolType == ProcessType::ReaderProcess ? ProcessType::WriterProcess : ProcessType::ReaderProcess,
                    std::bind(&IPCTool::timeout, this));
            }

            ~IPCTool()
            {
                if (procInfo)
                {
                    auto lk = procInfo->createScopedLock();

                    if (ipcToolType == ProcessType::ReaderProcess) {
                        procInfo->readerProcessCount--;
#if DEBUG
                        std::cout << "Decreased reader count: " + std::to_string(procInfo->readerProcessCount) + "\n";
#endif
                    }
                    else if (ipcToolType == ProcessType::WriterProcess){
                        procInfo->writerProcessCount--;
#if DEBUG
                        std::cout << "Decreased writer count: " + std::to_string(procInfo->writerProcessCount) + "\n";
#endif
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

            timer->startObserver();

            while(!fileReader->isReadFinished())
            {
                fileReader->read();
            }

            queue->close();
            timer = nullptr;
        }
        else if (ipcToolType == ProcessType::WriterProcess)
        {
            if (!fileWriter)
            {
                throw std::runtime_error("FileWriter is nullptr");
            }

            std::cout << "IPC: Writing the file...\n";

            fileWriter->create();

            timer->startObserver();

            while(true)
            {
                if (queue->isReadFinished() && queue->isEmpty())
                {
                    break;
                }

                fileWriter->write();
            }

            timer = nullptr;
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
    std::unique_ptr<SiblingProcessObserver> timer;

    void timeout()
    {
        std::cout << std::format("Exit because of the time out: no running {} process", ipcToolType == ProcessType::ReaderProcess ? "writer" : "reader") << std::endl;
        SharedMemManager->tryRemoveActiveSharedMemoryObject();
        std::terminate();
    }
};