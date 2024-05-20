#pragma once
#include <memory>
#include <csetjmp>
#include <csignal>

/// Class that handles abnormal program termination (terminate, SIGINT, SIGABRT, other signals may be added and handled here)
class ProcessTerminator
{
public:
    static std::shared_ptr<ProcessTerminator> getInstance()
    {
        static std::shared_ptr<ProcessTerminator> instance{new ProcessTerminator};
        return instance;
    }

    /// state of the stack to jump to in case of an exception/signal
    /// and properly call destructor for RAII data structures
    jmp_buf buffer;

private:
    /// Function handles terminate
    static void terminateHandler()
    {
#if DEBUG
        std::cout << "Terminate Handler\n";
#endif
        // Jump to the saved stack
        // To properly call destructors
        auto terminator = ProcessTerminator::getInstance();
        longjmp(terminator->buffer, 1);
    }

    /// Function handles signals
    /// Overload because signature incompatibility:
    /// terminate requires void (*) ()
    /// signals require void (*) (int signal)
    static void terminateHandler(int signal)
    {
#if DEBUG
        std::cout << "Signal Terminate Handler\n";
#endif
        // Jump to the saved stack
        // To properly call destructors
        auto terminator = ProcessTerminator::getInstance();
        longjmp(terminator->buffer, 1);
    }

    /// Constructor that sets handlers for terminate and signals
    /// Since the class is singleton, it happens once
    ProcessTerminator()
    {
        std::set_terminate(&ProcessTerminator::terminateHandler);
        std::signal(SIGABRT, terminateHandler);
        std::signal(SIGINT, terminateHandler);
    }
};