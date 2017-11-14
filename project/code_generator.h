#pragma once

#include "project/project.h"

#include <string>
#include <sstream>

namespace ss {

class CodeGenerator
{
public:
    explicit CodeGenerator(const Project& project);

	std::string ErrorMessage() const;

	bool Generate(std::string& outputPath);

private:
	bool GenerateFile(const ProjectMetadataFile& file, std::string& outputPath);

	const Project& m_project;
	std::stringstream m_error;
};

}
