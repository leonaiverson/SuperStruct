
#include "project/project_parser.h"

namespace ss
{

ProjectParser::ProjectParser()
{
}

ProjectParser::~ProjectParser()
{
}

void ProjectParser::Open( const char* text, size_t size )
{
	m_tokenizer.SetDefaultCharsets();
	m_tokenizer.SetCharacterRule( '=', base::PARSING_RULE_SYMBOL );
	m_tokenizer.SetCharacterRule( '{', base::PARSING_RULE_SYMBOL );
	m_tokenizer.SetCharacterRule( '}', base::PARSING_RULE_SYMBOL );

	m_tokenizer.Open( text, size );
}

void ProjectParser::Close()
{
	m_tokenizer.Close();
}

bool ProjectParser::SkipEndLines()
{
	do
	{
		if( !m_tokenizer.ParseNext() )
			return false;
	} while ( m_tokenizer.IsSymbol() && m_tokenizer.GetSymbol() == '\n' );

	return true;
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
				if( !ParseStringValue( project.m_name ) )
				{
					m_errorMessage << "for project.name";
					return false;
				}
			}
			else if( Cmp( "object", key ) )
			{
				if( !SkipEndLines() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '{' )
				{
					m_errorMessage << "'{' expected after 'object'";
					return false;
				}

				auto object = std::make_unique<ProjectObject>();

				if( !ParseObject( project, *object ) )
					return false;

				project.m_objects.push_back(std::move(object));
			}
		}

		if( !m_tokenizer.ParseNext() )
			process = false;
	} while( process );

	return true;
}

bool ProjectParser::ParseWordValue( String_t& value )
{
	if( !m_tokenizer.ParseNext() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '=' )
	{
		m_errorMessage << "=[Word identifier] expected ";
		return false;
	}

	if( !m_tokenizer.ParseNext() || !m_tokenizer.IsWord() )
	{
		m_errorMessage << "Word identifier expected ";
		return false;
	}
	
	value = m_tokenizer.GetWord();
	return true;
}

bool ProjectParser::ParseStringValue( String_t& value )
{
	if( !m_tokenizer.ParseNext() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '=' )
	{
		m_errorMessage << "=[Quoted string] expected ";
		return false;
	}

	if( !m_tokenizer.ParseNext() || !m_tokenizer.IsString() )
	{
		m_errorMessage << "Quoted string expected ";
		return false;
	}

	value = m_tokenizer.GetString();
	return true;
}

bool ProjectParser::ParseNumValue( String_t& value )
{
	if( !m_tokenizer.ParseNext() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '=' )
	{
		m_errorMessage << "=[Number] expected ";
		return false;
	}

	if( !m_tokenizer.ParseNext() || !m_tokenizer.IsNumber() )
	{
		m_errorMessage << "Number expected ";
		return false;
	}

	value = m_tokenizer.GetNumber();
	return true;
}

bool ProjectParser::ParseObject(Project& project, ProjectObject& object)
{
	if( !SkipEndLines() )
	{
		m_errorMessage << "Parameters expected after 'object {'";
		return false;
	}

	bool process = true;
	do
	{
		if( m_tokenizer.IsWord() )
		{
			const char* key = m_tokenizer.GetWord();
			if( Cmp( "name", key ) )
			{
				if( !ParseStringValue( object.m_name ) )
				{
					m_errorMessage << "for object.name";
					return false;
				}
			}
		}
		else if( m_tokenizer.IsSymbol() && m_tokenizer.GetSymbol() == '}' )
			return true;

		if( !m_tokenizer.ParseNext() )
		{
			m_errorMessage << "'}' expected for 'object'";
			process = false;
		}
	} while( process );

	return false;
}

String_t ProjectParser::ErrorMessage() const
{
	return m_errorMessage.str();
}

}
