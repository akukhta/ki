#include "common/OptionsParser.hpp"
#include "common/PseudoToolFactory.hpp"
#include "common/ProcessTerminator.hpp"

int main(int argc, char ** argv)
{	
	OptionsParser parser(argc, argv);
	
	ToolFactory toolFactory(parser, parser.getToolType());
	
	auto cp = toolFactory.createTool();

    /*
    if (setjmp(ProcessTerminator::getInstance()->buffer))
    {
        cp = nullptr;
        return 1;
    }
    */

	cp->copy();

	return 0;
}
