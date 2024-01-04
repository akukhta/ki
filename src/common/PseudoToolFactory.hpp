#pragma once 
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../queue/SynchronizedQueue.hpp"
#include "ParallelCopyTool.hpp"
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

            default:
            {
                std::cerr << "Unknown tool type" << std::endl;
                break;
            }
        }

        return tool;
    }       

private:
    IOptionsParser const &parser;
    ToolType type;
};