
#include "project/project_parser.h"

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
				if( !ParseEqualsWord( object.m_name ) )
				{
					m_errorMessage << "for object.name";
					return false;
				}
			}
			else if (Cmp("data", key))
			{
				if (!SkipEndLines() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '{')
				{
					m_errorMessage << "'{' expected after object.data";
					return false;
				}

				if (!ParseObjectData(project, object))
					return false;
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

bool ProjectParser::ParseObjectData(Project& project, ProjectObject& object)
{
	if (!SkipEndLines())
	{
		m_errorMessage << "Parameters expected after object.data {'";
		return false;
	}

	bool process = true;
	do
	{
		if (m_tokenizer.IsWord())
		{
			Name_t valueName;
			std::string valueType = m_tokenizer.GetWord();

			if (!ParseWord(valueName))
			{
				m_errorMessage << "for object.data.value name";
				return false;
			}

			if (Cmp("bool", valueType))
			{
				bool value = false;
				if (!ParseEqualsBool(value))
				{
					m_errorMessage << "for object.data.bool name=value";
					return false;
				}
			}
			else if (Cmp("int", valueType))
			{
				int value = 0;
				if (!ParseEqualsInt(value))
				{
					m_errorMessage << "for object.data.int name=value";
					return false;
				}
			}
			else if (Cmp("float", valueType))
			{
				float value = 0;
				if (!ParseEqualsFloat(value))
				{
					m_errorMessage << "for object.data.float name=value";
					return false;
				}
			}
			else {
				m_errorMessage << " bool, int, float types expected";
				return false;
			}
		}
		else if (m_tokenizer.IsSymbol() && m_tokenizer.GetSymbol() == '}')
			return true;

		if (!m_tokenizer.ParseNext())
		{
			m_errorMessage << "'}' expected for object.data";
			process = false;
		}
	} while (process);

	return false;
}

}
