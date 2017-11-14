
#include "project/project_metadata_parser.h"

namespace ss
{

ProjectMetadataParser::ProjectMetadataParser()
{
}

ProjectMetadataParser::~ProjectMetadataParser()
{
}

bool ProjectMetadataParser::Parse(ProjectMetadata& meta)
{
	if (!SkipEndLines())
		return false;

	bool process = true;
	do
	{
		if (m_tokenizer.IsWord())
		{
			const char* key = m_tokenizer.GetWord();
			if (Cmp("view", key))
			{
				auto view = std::make_unique<ProjectObjView>();
				if (!ParseWord(view->m_name))
				{
					m_errorMessage << "for view name";
					return false;
				}

				if (!SkipEndLines() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '{')
				{
					m_errorMessage << "'{' expected after 'view name'";
					return false;
				}

				if (!ParseObjView(meta, *view))
					return false;

				meta.m_views.push_back(std::move(view));
			}
		}

		if (!m_tokenizer.ParseNext())
			process = false;
	} while (process);

	return true;
}

bool ProjectMetadataParser::ParseObjView(ProjectMetadata& meta, ProjectObjView& view)
{
	if (!SkipEndLines())
	{
		m_errorMessage << "Parameters expected after 'view {'";
		return false;
	}

	bool process = true;
	do
	{
		if (m_tokenizer.IsWord())
		{
			const char* key = m_tokenizer.GetWord();
			if (Cmp("layout", key))
			{
				if (!SkipEndLines() || !m_tokenizer.IsSymbol() || m_tokenizer.GetSymbol() != '{')
				{
					m_errorMessage << "'{' expected after 'view'";
					return false;
				}

				if (!ParseObjViewLayout(meta, view)) 
				{
					m_errorMessage << " for view layout " << view.m_name;
					return false;
				}
			}
		}
		else if (m_tokenizer.IsSymbol() && m_tokenizer.GetSymbol() == '}')
			return true;

		if (!m_tokenizer.ParseNext())
		{
			m_errorMessage << "'}' expected for 'view'";
			process = false;
		}
	} while (process);

	return false;
}

bool ProjectMetadataParser::ParseObjViewLayout(ProjectMetadata& meta, ProjectObjView& view)
{
	if (!SkipEndLines())
	{
		m_errorMessage << "Parameters expected after view.layout {'";
		return false;
	}

	bool process = true;
	do
	{
		if (m_tokenizer.IsWord())
		{
			Name_t memberName;
			std::string memberType = m_tokenizer.GetWord();

			if (!ParseWord(memberName))
			{
				m_errorMessage << "for view.layout type memberName";
				return false;
			}

			ValueType valueType;
			if (Cmp("bool", memberType))
			{
				valueType = ValueType::Bool;
			}
			else if (Cmp("int", memberType))
			{
				valueType = ValueType::Int;
			}
			else if (Cmp("float", memberType))
			{
				valueType = ValueType::Float;
			}
			else if (Cmp("Vec3", memberType))
			{
				valueType = ValueType::Vec3;
			}
			else {
				m_errorMessage << "member " << memberName << ": bool, int, float, Vec3 types expected";
				return false;
			}

			auto kv = view.m_members.find(memberName);
			if (kv == view.m_members.end())
			{
				view.m_members.insert(std::make_pair(memberName, valueType));
			}
			else
			{
				m_errorMessage << " view already contains member " << kv->first;
				return false;
			}
		}
		else if (m_tokenizer.IsSymbol() && m_tokenizer.GetSymbol() == '}')
			return true;

		if (!m_tokenizer.ParseNext())
		{
			m_errorMessage << "'}' expected for view.layout";
			process = false;
		}
	} while (process);

	return false;
}

}
