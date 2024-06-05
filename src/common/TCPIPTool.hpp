#pragma once
#include <vector>
#include <future>
#include "ICopyTool.hpp"
#include "StopWatch.h"
#include "../TCPIP/Server/MultiFileWriter.hpp"
#include "../TCPIP/Client/BufferedFileReader.h"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/Server/IServer.hpp"
#include "../TCPIP/Client/IClient.hpp"
#include "../TCPIP/Server/TCPIPServer.hpp"
#include "CLILoadIndicator.hpp"

class TCPIPTool : public ICopyTool
{
public:
    explicit TCPIPTool(std::shared_ptr<TCPIP::MultiFileWriter> fileWriter, std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::unique_ptr<TCPIP::IServer> server, bool showMemoryPoolUsage = true, int indicatorRefreshRate = 0)
            :  fileWriter(std::move(fileWriter)), queue(std::move(queue)),
               server(std::move(server)), sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark")),
               showMemoryPoolUsage(showMemoryPoolUsage), indicatorRefreshRate(indicatorRefreshRate)
    {
    }

    explicit TCPIPTool(std::unique_ptr<TCPIP::BufferedReader> fileReader,
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<TCPIP::IClient> client, std::vector<std::string> filesToSend)
            :    fileReader(std::move(fileReader)), queue(std::move(queue)),
                 client(std::move(client)), filesToSend(std::move(filesToSend)),
                 sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
    {
    }

    ~TCPIPTool()
    {
        if (memoryPoolUsageBar)
        {
            guiStopToken.request_stop();
        }
    }

    void copy() override
    {
        if (client)
        {
            for (auto const & file: filesToSend)
            {
                queue->open();
                queue->startReading();
                auto readingTask = std::async(std::launch::async, &TCPIPTool::read, this, file);
                client->sendFile(file);
                readingTask.get();
            }
        }
        else if (server)
        {
            std::future<void> guiTask;

            if (showMemoryPoolUsage)
            {
                memoryPoolUsageBar = std::make_unique<UI::CLILoadIndicator>("Buffers available:", TCP_BUFFERS_IN_QUEUE, TCP_BUFFERS_IN_QUEUE);
                guiTask = std::async(std::launch::async, &TCPIPTool::updateProgressBar, this);
            }

            server->run();
            writingFunction();

            if (guiTask.valid())
            {
                guiStopToken.request_stop();
                guiTask.wait();
            }
        }
    }

private:

    StopWatch sw;
    std::shared_ptr<TCPIP::MultiFileWriter> fileWriter;
    std::unique_ptr<TCPIP::BufferedReader> fileReader;
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::unique_ptr<TCPIP::IServer> server;
    std::unique_ptr<TCPIP::IClient> client;
    std::unique_ptr<UI::CLILoadIndicator> memoryPoolUsageBar;
    std::vector<std::string> filesToSend;
    std::jthread fileioThread;
    std::stop_source guiStopToken;

    bool showMemoryPoolUsage = false;
    int indicatorRefreshRate = 0;

    void read(std::string const &file)
    {
        fileReader->startReading(file);

        while (!fileReader->isReadFinished())
        {
            fileReader->read();
        }

        fileReader->finishRead();
        queue->finishRead();
        queue->close();
    }

    void writingFunction()
    {
        queue->open();

        while (true)
        {
            fileWriter->write();
        }
    }

    void updateProgressBar()
    {
        // Since queue is not polymorphic, no dynamic_cast/dynamic_pointer_cast allowed
        auto serverQeuee = static_cast<TCPIP::FixedBufferQueue*>(queue.get());

        while (!guiStopToken.stop_requested())
        {
            memoryPoolUsageBar->setValue(serverQeuee->getFreeBuffersAmount());
            memoryPoolUsageBar->draw();
            std::this_thread::sleep_for(std::chrono::milliseconds(indicatorRefreshRate));
        }
    }
};