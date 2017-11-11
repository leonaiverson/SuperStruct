#pragma once

#include "project/project.h"

#include "base/parser.h"
#include "base/tokenizer.h"

#include <string>
#include <sstream>
#include <vector>

namespace ss
{

using String_t = std::string;
using StringStream_t = std::stringstream;

class ProjectParser : public base::Parser
{
public:
	ProjectParser();
	~ProjectParser();
	
	void Open( const char* text, size_t size );
	void Close();

	bool Parse( Project& project );
	String_t ErrorMessage() const;

private:
	bool ParseWordValue( String_t& value );
	bool ParseStringValue( String_t& value );
	bool ParseNumValue( String_t& value );
		
	bool ParseObject(Project& project, ProjectObject& object);
	bool SkipEndLines();

	base::TextTokenizer m_tokenizer;
		
	StringStream_t m_errorMessage;
};

}
