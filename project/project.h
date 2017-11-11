#pragma once

#include "base/base.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ss {

using Name_t = std::string;

enum class ValueType : int {
	Bool, Int, Float
};

class ProjectObject {
public:
	Name_t m_name;

	using Value_t = std::variant<bool, int, float>; // indices must match ValueType 

	using Values_t = std::unordered_map<Name_t, Value_t>;
	Values_t m_values;
};

class Project {
public:
	Name_t m_name;

	using Objects_t = std::vector<std::unique_ptr<ProjectObject>>;
	Objects_t m_objects;
};

}
