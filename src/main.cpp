#include "common/OptionsParser.hpp"
#include "common/PseudoToolFactory.hpp"

int main(int argc, char ** argv)
{	
	OptionsParser parser(argc, argv);
	
	ToolFactory toolFactory(parser, parser.getToolType());
	
	auto cp = toolFactory.createTool();

	cp->copy();

	return 0;
}