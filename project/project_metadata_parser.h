#pragma once

#include "project/project.h"

#include "base/parser.h"

namespace ss
{

class ProjectMetadataParser : public base::Parser
{
public:
	ProjectMetadataParser();
	~ProjectMetadataParser();

	bool Parse(ProjectMetadata& project);

private:
	bool ParseObjView(ProjectMetadata& meta, ProjectObjView& view);
	bool ParseObjViewLayout(ProjectMetadata& meta, ProjectObjView& view);
};

}
