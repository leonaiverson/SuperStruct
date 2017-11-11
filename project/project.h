#pragma once

#include "base/base.h"

#include <memory>
#include <string>
#include <vector>

namespace ss {

using Name_t = std::string;

class ProjectObject {
public:
	Name_t m_name;
};

class Project {
public:
	Name_t m_name;

	using Objects_t = std::vector<std::unique_ptr<ProjectObject>>;
	Objects_t m_objects;
};

}
