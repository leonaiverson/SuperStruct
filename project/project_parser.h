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
	bool ReadData(const String_t& dataFileName, ProjectData& data);
};

}
