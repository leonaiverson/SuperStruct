
#include "base/base.h"
#include "base/memoryblock.h"
#include "project/project_parser.h"

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

	std::string fileName = argv[1];

	try
	{
		ss::Project project;
		{
			base::MemoryBlock projectFileContent;
			if (!base::ReadWholeFile(fileName, projectFileContent))
			{
				out << "Can't read file " << fileName << std::endl;
				return false;
			}

			ss::ProjectParser parser;
			parser.Open((const char*)projectFileContent.pBlock, projectFileContent.size);
			if (!parser.Parse(project))
			{
				out << "Can't parse file " << fileName << ". parse error: " << parser.ErrorMessage() << std::endl;
				return false;
			}
			parser.Close();
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "\n";
	}

	out << "All good!" << std::endl;
    return 0;
}

