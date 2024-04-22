#pragma once

/// The mode in which IPC process works:
/// reader, writer or invalid (an error has occurred)
enum class ProcessType : char {ReaderProcess, WriterProcess, Invalid};
