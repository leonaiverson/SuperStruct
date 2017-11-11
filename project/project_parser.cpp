
#include "project/project_parser.h"
#include "project/project_data_parser.h"

#include "base/memoryblock.h"

namespace ss
{

ProjectParser::ProjectParser()
{
}

ProjectParser::~ProjectParser()
{
}

bool ProjectParser::Parse(Project& project)
{
	if( !SkipEndLines() )
		return false;

	bool process = true;
	do
	{
		if( m_tokenizer.IsWord() )
		{
			const char* key = m_tokenizer.GetWord();
			if( Cmp( "name", key ) )
			{
				if( !ParseEqualsString( project.m_name ) )
				{
					m_errorMessage << "for project.name";
					return false;
				}
			}
			else if( Cmp( "data", key ) )
			{
				String_t dataFileName;
				if (!ParseEqualsString(dataFileName))
				{
					m_errorMessage << "for project.data";
					return false;
				}

				if (!ReadData(dataFileName, project.m_data))
					return false;
			}
		}

		if( !m_tokenizer.ParseNext() )
			process = false;
	} while( process );

	return true;
}

bool ProjectParser::ReadData(const String_t& fileName, ProjectData& data)
{
	base::MemoryBlock content;
	if (!base::ReadWholeFile(fileName, content))
	{
		m_errorMessage << "Can't read file " << fileName << std::endl;
		return false;
	}

	ss::ProjectDataParser dataParser;
	dataParser.Open(content.pBlock, content.size);
	if (!dataParser.Parse(data))
	{
		m_errorMessage << "Can't parse data file " << fileName << ". parse error: " << dataParser.ErrorMessage() << std::endl;
		return false;
	}

	return true;
}

}
