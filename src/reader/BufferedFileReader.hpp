#pragma once
#include "IFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include <filesystem>
#include <cstdio>
#include <utility>

class BufferedReader : public IFileReader<void>
{
public:
    explicit BufferedReader(std::string fileName, std::shared_ptr<FixedBufferQueue> queue);

    void open() override;

    void read() override;

    bool isReadFinished() const override;

    void finishRead() override;
    
    size_t getFileSize() const;

    ~BufferedReader();

    static inline size_t defaultBufferSize = sysconf(_SC_PAGESIZE);

private:
    std::FILE* fileDesc = nullptr;

    size_t fileSize = 0;
    
    size_t currentOffset = 0;
    std::atomic_bool readFinished{false};

    std::string fileName;
    std::shared_ptr<FixedBufferQueue> queue;
};