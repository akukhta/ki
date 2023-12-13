#include <iostream>
#include <new>
#include "ParallelCopyTool.hpp"
#include "OptionsParser.hpp"
#include "../queue/SynchronizedQueue.hpp"
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"

int main(int argc, char ** argv)
{
	OptionsParser parser(argc, argv);
	
	auto reader = std::make_unique<MMapFileReader>(std::move(parser.getSrc()));
	auto writer = std::make_unique<MMapFileWriter>(std::move(parser.getDst()), reader->getFileSize());
	auto queue = std::make_shared<SynchronizedQueue>();

	ParallelCopyTool cp(std::move(reader), std::move(writer), queue);
	cp.copy();

	return 0;
}
