#include <iostream>
#include <new>
#include "ParallelCopyTool.hpp"
#include "OptionsParser.hpp"
#include "../queue/SynchronizedQueue.hpp"
#include "PseudoToolFactory.hpp"
#include "../reader/BufferedFIleReader.hpp"

int main(int argc, char ** argv)
{
	BufferedReader reader;
	
	OptionsParser parser(argc, argv);
	
	ToolFactory toolFactory(parser, ToolType::PARALLEL);
	
	auto cp = toolFactory.createTool();

	cp->copy();

	return 0;
}
