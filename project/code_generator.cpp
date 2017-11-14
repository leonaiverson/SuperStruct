#include "project/code_generator.h"

#include "external/mustache/mustache.hpp"

#include <experimental/filesystem>
#include <fstream>

namespace ss {

namespace ms = kainjow::mustache;
namespace fs = std::experimental::filesystem::v1;

CodeGenerator::CodeGenerator(const Project& project)
	: m_project(project)
{
}

bool CodeGenerator::Generate(std::string& outputPath)
{
	fs::path outputName = fs::path(outputPath) / m_project.m_metadataFileName;
	outputName.replace_extension("h");
	std::ofstream output(outputName.native());

	fs::path textName = m_project.m_metadataFileName;
	textName.replace_extension("ss_h");
	if (!fs::exists(textName)) {
		m_error << "Text template " << textName << " doesn't exist";
		return false;
	}

	std::ifstream textStream(textName.native());
	std::string textContent;

	textStream.seekg(0, std::ios::end);   
	textContent.reserve(textStream.tellg());
	textStream.seekg(0, std::ios::beg);

	textContent.assign((std::istreambuf_iterator<char>(textStream)),
		std::istreambuf_iterator<char>());

	ms::mustache text(textContent);
	ms::data parameters(ms::data::type::list);
	text.render(parameters, output);

	return true;
}

std::string CodeGenerator::ErrorMessage() const
{
	return m_error.str();
}

}
