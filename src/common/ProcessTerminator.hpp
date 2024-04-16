#pragma once
#include <memory>
#include <setjmp.h>
#include <csignal>

class ProcessTerminator
{
public:
    static std::shared_ptr<ProcessTerminator> getInstance()
    {
        static std::shared_ptr<ProcessTerminator> instance{new ProcessTerminator};
        return instance;
    }

    jmp_buf buffer;

private:
    static void terminateHandler()
    {
        std::cout << "Terminate Handler\n";
        auto terminator = ProcessTerminator::getInstance();
        longjmp(terminator->buffer, 1);
    }

    static void terminateHandler(int signal)
    {
        std::cout << "Signal Terminate Handler\n";
        auto terminator = ProcessTerminator::getInstance();
        longjmp(terminator->buffer, 1);
    }

    ProcessTerminator()
    {
        std::set_terminate(&ProcessTerminator::terminateHandler);
        std::signal(SIGABRT, terminateHandler);
        std::signal(SIGINT, terminateHandler);
    }
};