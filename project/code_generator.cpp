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
	for (auto& file : m_project.m_metadataFiles) {
		if (!GenerateFile(*file, outputPath))
			return false;
	}

	return true;
}

bool CodeGenerator::GenerateFile(const ProjectMetadataFile& file, std::string& outputPath)
{
	fs::path fileName = file.m_name;
	fs::path outputName = fs::path(outputPath) / fileName.filename();
	outputName.replace_extension("inl");

	fs::create_directories(outputName.parent_path());

	std::ofstream output(outputName.native());

	fs::path textName = file.m_name;
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
	ms::data parameters;

    ms::data views(ms::data::type::list);
	for (auto& v : file.m_meta.m_views) {
		ms::data view;
	    view.set("name", v->m_name);

	    ms::data members(ms::data::type::list);
		for (auto& m : v->m_members) {
			ms::data member;
			member.set("name", m.first);
			member.set("type", ToString(m.second));
			members.push_back(member);
		}
		view.set("members", members);

		views.push_back(view);
	}
	parameters.set("views", views);

	text.render(parameters, output);
	
	bool success = text.is_valid();
	if (!success)
		m_error << text.error_message();

	return success;
}

std::string CodeGenerator::ErrorMessage() const
{
	return m_error.str();
}

}
