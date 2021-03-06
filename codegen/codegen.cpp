
#include "base/base.h"
#include "base/memoryblock.h"
#include "project/code_generator.h"
#include "project/project_parser.h"
#include "project/solver.h"

#include <iostream>

int main(int argc, const char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::ostream& out = std::cout;

	if (argc < 3)
	{
		out << "Usage: <project.ssproj> <output_path>\n";
		return 1;
	}

	try
	{
		std::string fileName = argv[1];
		std::string outputPath = argv[2];
		
		ss::Project project;
		project.m_projectPath = ss::FindParentPath(fileName);
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

		{
			ss::LayoutSolver solver(project);
			ss::LayoutSolver::BestViews_t bestViews;
			if (!solver.Solve(bestViews))
			{
				out << "Layout solver error: " << solver.ErrorMessage() << std::endl;
				return 4;
			}
		}

		{
			ss::CodeGenerator generator(project);
			if (!generator.Generate(outputPath))
			{
				out << "Code generator error: " << generator.ErrorMessage() << std::endl;
				return 5;
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	out << "All good!" << std::endl;
	return 0;
}

