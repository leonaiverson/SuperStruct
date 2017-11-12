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

const std::string& ToString(ValueType type);

class ProjectObjView {
public:
	Name_t m_name;

	using Members_t = std::unordered_map<Name_t, ValueType>;
	Members_t m_members;
};

class ProjectMetadata {
public:
	using Views_t = std::vector<std::unique_ptr<ProjectObjView>>;
	Views_t m_views;
};


class ProjectObject {
public:
	Name_t m_name;

	using Value_t = std::variant<bool, int, float>; // indices must match ValueType 

	using Values_t = std::unordered_map<Name_t, Value_t>;
	Values_t m_values;
};

class ProjectData {
public:
	using Objects_t = std::vector<std::unique_ptr<ProjectObject>>;
	Objects_t m_objects;
};

class Project {
public:
	Name_t m_name;

	ProjectMetadata m_meta;
	ProjectData m_data;
};

}
