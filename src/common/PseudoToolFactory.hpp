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

/// Factory that creates proper copy tool based on passed arguments
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

                // If the copy tool process is first, we create the tool that works as a reader
                if (shMemManager->isFirstProcess())
                {
                    // Fill the reader process information
                    auto lock = procInfo->createScopedLock();
                    procInfo->dst = parser.getDst();
                    procInfo->isWritingStarted = false;
                }

                // Initially, tool type is set to invalid
                // because we should check amount of process instances
                // If 0 readers, we create a reader
                // If we already have the reader and no writer, we create the writer
                // if we already have the reader and the writer, the tool type remains invalid and will be terminated
                // as we run copy function
                ProcessType toolType = ProcessType::Invalid;
                {
                    auto lock =  procInfo->createScopedLock();

                    if (procInfo->readerProcessCount <= procInfo->writerProcessCount)
                    {
                        // For reader process there is no need to create writer object, so the writer is null
                        reader = std::make_unique<BufferedReader<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>>(
                                std::move(parser.getSrc()), queue);
                        toolType = ProcessType::ReaderProcess;
                    }
                    else
                    {
                        // For writer process there is no need to create reader object, so the reader is null
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