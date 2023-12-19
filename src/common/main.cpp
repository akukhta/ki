#include <iostream>
#include <new>
#include "ParallelCopyTool.hpp"
#include "OptionsParser.hpp"
#include "../queue/SynchronizedQueue.hpp"
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "PseudoToolFactory.hpp"

int main(int argc, char ** argv)
{
	OptionsParser parser(argc, argv);
	
	ToolFactory toolFactory(parser, ToolType::PARALLEL);
	
	auto cp = toolFactory.createTool();

	cp->copy();

	return 0;
}
