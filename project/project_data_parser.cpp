
#include "project/project_data_parser.h"

namespace ss
{

ProjectDataParser::ProjectDataParser()
{
}

ProjectDataParser::~ProjectDataParser()
{
}

bool ProjectDataParser::Parse(ProjectData& project)
{
	if (!SkipEndLines())
		return false;

	bool process = true;
	do
	{
		if (m_tokenizer.IsWord())
		{
			const char* key = m_tokenizer.GetWord();
			if (Cmp("object", key))
			{
				if (!SkipEndLines() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '{')
				{
					m_errorMessage << "'{' expected after 'object'";
					return false;
				}

				auto object = std::make_unique<ProjectObject>();

				if (!ParseObject(project, *object))
					return false;

				project.m_objects.push_back(std::move(object));
			}
		}

		if (!m_tokenizer.ParseNext())
			process = false;
	} while (process);

	return true;
}

bool ProjectDataParser::ParseObject(ProjectData& project, ProjectObject& object)
{
	if (!SkipEndLines())
	{
		m_errorMessage << "Parameters expected after 'object {'";
		return false;
	}

	bool process = true;
	do
	{
		if (m_tokenizer.IsWord())
		{
			const char* key = m_tokenizer.GetWord();
			if (Cmp("name", key))
			{
				if (!ParseEqualsWord(object.m_name))
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
		else if (m_tokenizer.IsSymbol() && m_tokenizer.GetSymbol() == '}')
			return true;

		if (!m_tokenizer.ParseNext())
		{
			m_errorMessage << "'}' expected for 'object'";
			process = false;
		}
	} while (process);

	return false;
}

bool ProjectDataParser::ParseObjectData(ProjectData& project, ProjectObject& object)
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

			ProjectObject::Value_t projectValue;

			if (Cmp("bool", valueType))
			{
				bool value = false;
				if (!ParseEqualsBool(value))
				{
					m_errorMessage << "for object.data.bool name=value";
					return false;
				}
				projectValue = value;
			}
			else if (Cmp("int", valueType))
			{
				int value = 0;
				if (!ParseEqualsInt(value))
				{
					m_errorMessage << "for object.data.int name=value";
					return false;
				}
				projectValue = value;
			}
			else if (Cmp("float", valueType))
			{
				float value = 0;
				if (!ParseEqualsFloat(value))
				{
					m_errorMessage << "for object.data.float name=value";
					return false;
				}
				projectValue = value;
			}
			else {
				m_errorMessage << " bool, int, float types expected";
				return false;
			}

			auto kv = object.m_values.find(valueName);
			if (kv == object.m_values.end())
			{
				object.m_values.insert(std::make_pair(valueName, projectValue));
			}
			else
			{
				m_errorMessage << " object already contains value " << kv->first;
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
