#pragma once

#include "project/project.h"

#include "base/parser.h"

namespace ss
{

class ProjectParser : public base::Parser
{
public:
	ProjectParser();
	~ProjectParser();
	
	bool Parse( Project& project );

private:
	bool ParseObject(Project& project, ProjectObject& object);
	bool ParseObjectData(Project& project, ProjectObject& object);
};

}
