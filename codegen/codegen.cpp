
#include "base/base.h"
#include "base/memoryblock.h"
#include "project/project_parser.h"
#include "project/solver.h"

#include <iostream>

int main(int argc, const char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::ostream& out = std::cout;

	if (argc < 2)
	{
		out << "No input project specified\n";
		return 1;
	}

	try
	{
		std::string fileName = argv[1];
		ss::Project project;
		{
			base::MemoryBlock content;
			if (!base::ReadWholeFile(fileName, content))
			{
				out << "Can't read file " << fileName << std::endl;
				return 2;
			}

			ss::ProjectParser parser;
			parser.Open(content.pBlock, content.size);
			if (!parser.Parse(project))
			{
				out << "Can't parse file " << fileName << std::endl;
				out << "Parse error: " << parser.ErrorMessage() << std::endl;
				return 3;
			}
		}

		ss::LayoutSolver solver(project, out);
		if (!solver.Solve())
		{
			return 4;
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	out << "All good!" << std::endl;
	return 0;
}

