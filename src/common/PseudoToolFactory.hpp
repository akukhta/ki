#pragma once 
#include "../reader/MMapFileReader.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../queue/SynchronizedQueue.hpp"
#include "../queue/BufferedQueue.hpp"
#include "ParallelCopyTool.hpp"
#include "BufferedParallelCopyTool.hpp"
#include "ToolTypeEnum.hpp"
#include "IOptionsParser.hpp"
#include "IPCTool.hpp"
#include <memory>
#include <iostream>
#include <vector>
#include <boost/interprocess/interprocess_fwd.hpp>

class ToolFactory
{
public:
    ToolFactory (IOptionsParser const &parser, ToolType type) 
        : parser(parser), type(type) {}

    virtual std::unique_ptr<ICopyTool> createTool()
    {
        std::unique_ptr<ICopyTool> tool = nullptr;

        switch (type)
        {
            case ToolType::PARALLEL:
            {
                	auto reader = std::make_unique<MMapFileReader<std::vector<unsigned char>>>(std::move(parser.getSrc()));
	                
                    auto writer = std::make_unique<MMapFileWriter<std::vector<unsigned char>>>(std::move(parser.getDst()),
                        reader->getFileSize());
	                
                    std::unique_ptr<IQueue<std::vector<unsigned char>>> queue =
                         std::make_unique<SynchronizedQueue<std::vector<unsigned char>>>();

                    tool = std::make_unique<ParallelCopyTool<std::vector<unsigned char>>>(
                        std::move(reader), std::move(writer), std::move(queue));
                    break;
            }

            case ToolType::BUFFERED_PARALLEL:
            {
                auto queue = std::make_shared<FixedBufferQueue<std::mutex, std::condition_variable, std::unique_lock, std::deque>>();

                auto reader = std::make_unique<BufferedReader<std::mutex, std::condition_variable, std::unique_lock, std::deque>>(std::move(parser.getSrc()), queue);

                auto writer = std::make_unique<BufferedFileWriter<std::mutex, std::condition_variable, std::unique_lock, std::deque>>(std::move(parser.getDst()), queue);

                tool = std::make_unique<BPCopyTool>(std::move(reader), std::move(writer), queue);
                
                break;
            }

            case ToolType::IPC:
            {
                auto shMemManager = std::make_shared<SharedMemoryManager>(parser.getSharedObjName());

                auto queue = shMemManager->getQueue("shQueue");

                std::unique_ptr<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> reader = nullptr;

                std::unique_ptr<BufferedFileWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>> writer = nullptr;

                auto procInfo = shMemManager->getProcInfo();

                if (shMemManager->isFirstProcess())
                {
                    auto lock = procInfo->createScopedLock();
                    procInfo->dst = parser.getDst();
                    procInfo->isWritingStarted = false;
                }
                else
                {
                    auto lock = procInfo->createScopedLock();
                    procInfo->isWritingStarted = true;
                }

                ProcessType toolType = ProcessType::Invalid;

                {
                    auto lock =  procInfo->createScopedLock();

                    if (procInfo->readerProcessCount <= procInfo->writerProcessCount)
                    {
                        reader = std::make_unique<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>>(
                                std::move(parser.getSrc()), queue);
                        toolType = ProcessType::ReaderProcess;
                    }
                    else
                    {
                        writer = std::make_unique<BufferedFileWriter<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>>(
                                procInfo->getDst(), queue);
                        toolType = ProcessType::WriterProcess;
                    }
                }

                tool = std::make_unique<IPCTool>(std::move(reader), std::move(writer), queue, shMemManager, toolType);

                break;
            }

            default:
            {
                std::cerr << "Unknown tool bufferType" << std::endl;
                break;
            }
        }

        return tool;
    }       

private:
    IOptionsParser const &parser;
    ToolType type;
};