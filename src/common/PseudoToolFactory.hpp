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
#include <memory>
#include <iostream>
#include <vector>
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
                auto queue = std::make_shared<FixedBufferQueue>();

                auto reader = std::make_unique<BufferedReader>(std::move(parser.getSrc()), queue);

                auto writer = std::make_unique<BufferedFileWriter>(std::move(parser.getDst()), queue);

                tool = std::make_unique<BPCopyTool>(std::move(reader), std::move(writer), queue);
                
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