#pragma once 
#include "../reader/MMapFileReader.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../TCPIP/Server/MultiFileWriter.hpp"
#include "../queue/SynchronizedQueue.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/Server/TCPIPServer.hpp"
#include "../TCPIP/Client/TCPIPClient.hpp"
#include "ParallelCopyTool.hpp"
#include "BufferedParallelCopyTool.hpp"
#include "TCPIPTool.hpp"
#include "ToolTypeEnum.hpp"
#include "IOptionsParser.hpp"
#include "IPCTool.hpp"
#include <memory>
#include <iostream>
#include <vector>
#include <utility>
#include <boost/interprocess/interprocess_fwd.hpp>
#include "../TCPIP/Client/BufferedFileReader.h"
#include "../TCPIP/Request/RequestHandler.hpp"
#include "../TCPIP/Common/JsonSettingsParser.hpp"
#include "../TCPIP/Client/TCPClientCommunication.hpp"
#include "../TCPIP/SecureClient/SecureClient.hpp"
#include "../TCPIP/Security/ChaCha20Encryption.hpp"
#include "../TCPIP/Security/SecureRequestHandler.hpp"
#include "../TCPIP/Security/KeyManager.hpp"

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
                auto queue = std::make_shared<FixedBufferQueue<NonIPCTag>>();

                auto reader = std::make_unique<BufferedReader<NonIPCTag>>(std::move(parser.getSrc()), queue);

                auto writer = std::make_unique<BufferedFileWriter<NonIPCTag>>(std::move(parser.getDst()), queue);

                tool = std::make_unique<BPCopyTool>(std::move(reader), std::move(writer), queue);

                break;
            }

            case ToolType::IPC:
            {
                auto shMemManager = std::make_shared<SharedMemoryManager>(parser.getSharedObjName());

                auto queue = shMemManager->getQueue("shQueue");

                std::unique_ptr<BufferedReader<IPCTag>> reader = nullptr;

                std::unique_ptr<BufferedFileWriter<IPCTag>> writer = nullptr;

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
                        reader = std::make_unique<BufferedReader<IPCTag>>(
                                std::move(parser.getSrc()), queue);
                        toolType = ProcessType::ReaderProcess;
                    }
                    else
                    {
                        // For writer process there is no need to create reader object, so the reader is null
                        writer = std::make_unique<BufferedFileWriter<IPCTag>>(
                                procInfo->getDst(), queue);
                        toolType = ProcessType::WriterProcess;
                    }
                }

                tool = std::make_unique<IPCTool>(std::move(reader), std::move(writer), queue, shMemManager, toolType);

                break;
            }

            case ToolType::TCPIPTOOL: {
                std::shared_ptr<TCPIP::MultiFileWriter> writer = nullptr;

                std::unique_ptr<TCPIP::TCPIPServer> server = nullptr;
                std::unique_ptr<TCPIP::IClient> client = nullptr;

                auto settingsParser = TCPIP::JsonSettingsParser::getInstance();
                settingsParser->setSettingsPath(std::move(parser.getSettingsPath()));

                if (parser.getIsServer())
                {
                    auto fileLogger = std::make_shared<FileLogger>();
                    auto queue = std::make_shared<TCPIP::FixedBufferQueue>();
                    writer = std::make_shared<TCPIP::MultiFileWriter>(queue, fileLogger);
                    auto requestHandler = std::make_unique<TCPIP::RequestHandler>(queue, writer, fileLogger);
                    server = std::make_unique<TCPIP::TCPIPServer>(queue, std::move(requestHandler), fileLogger);
                    tool = std::make_unique<TCPIPTool>(writer, queue, std::move(server), settingsParser->loadIndicatorEnabled(), settingsParser->loadInidicatorRefreshRate());
                    writer->setFileWriteFinished(std::bind(&TCPIP::TCPIPServer::fileWriteFinished, server.get(), std::placeholders::_1));
                }
                else
                {
                    auto tcpCommunication = std::make_unique<TCPIP::TCPClientCommunication>(std::move(settingsParser->getServerIP()), settingsParser->getServerPort());
                    auto files = parser.getFilesToSend();
                    auto queue = std::make_shared<FixedBufferQueue<TCPIPTag>>();
                    auto reader = std::make_unique<TCPIP::BufferedReader>(queue);
                    client = std::make_unique<TCPIP::TCPIPClient>(std::move(tcpCommunication), queue);
                    tool = std::make_unique<TCPIPTool>(std::move(reader), queue, std::move(client), std::move(files));
                }

                break;
            }

            case ToolType::SECURETCPIPTOOL:
            {
                std::shared_ptr<TCPIP::MultiFileWriter> writer = nullptr;

                std::unique_ptr<TCPIP::TCPIPServer> server = nullptr;
                std::unique_ptr<TCPIP::IClient> client = nullptr;

                auto settingsParser = TCPIP::JsonSettingsParser::getInstance();
                settingsParser->setSettingsPath(std::move(parser.getSettingsPath()));

                if (parser.getIsServer())
                {
                    auto fileLogger = std::make_shared<FileLogger>();
                    auto queue = std::make_shared<TCPIP::FixedBufferQueue>();
                    writer = std::make_shared<TCPIP::MultiFileWriter>(queue, fileLogger);
                    auto keyManager = std::make_shared<TCPIP::KeyManager>();
                    auto secureRequestHandler = std::make_unique<TCPIP::SecureRequestHandler>(queue, writer, std::make_unique<TCPIP::ChaCha20Encryption>(), std::move(keyManager), fileLogger);
                    server = std::make_unique<TCPIP::TCPIPServer>(queue, std::move(secureRequestHandler), fileLogger);
                    tool = std::make_unique<TCPIPTool>(writer, queue, std::move(server), settingsParser->loadIndicatorEnabled(), settingsParser->loadInidicatorRefreshRate());
                    writer->setFileWriteFinished(std::bind(&TCPIP::TCPIPServer::fileWriteFinished, server.get(), std::placeholders::_1));
                }
                else
                {
                    auto tcpCommunication = std::make_unique<TCPIP::TCPClientCommunication>(std::move(settingsParser->getServerIP()), settingsParser->getServerPort());
                    auto files = parser.getFilesToSend();
                    auto queue = std::make_shared<FixedBufferQueue<TCPIPTag>>();
                    auto reader = std::make_unique<TCPIP::BufferedReader>(queue);
                    client = std::make_unique<TCPIP::SecureTCPIPClient>(std::move(tcpCommunication), queue, std::make_unique<TCPIP::ChaCha20Encryption>(), std::make_unique<TCPIP::Chacha20Key>(TCPIP::Chacha20Key::generateRandomKey()));
                    tool = std::make_unique<TCPIPTool>(std::move(reader), queue, std::move(client), std::move(files));
                }

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